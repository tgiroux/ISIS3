// vim: ft=groovy

def isisDataPath = '/isisData/data'
def isisTestDataPath = "/isisData/testData"

def isisEnv = [
    "ISIS3DATA=${isisDataPath}",
    "ISIS3TESTDATA=${isisTestDataPath}"
]

def cmakeFlags = [
    "-DJP2KFLAG=OFF",
    "-Dpybindings=OFF",
    "-DCMAKE_BUILD_TYPE=RELEASE"
]

def build_ok = true

node("${env.OS.toLowerCase()}") {
    stage ("Checkout") {
        env.STAGE_STATUS = "Checking out ISIS"
        checkout scm
        isisEnv.add("ISISROOT=${pwd()}/build")
        cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
    }

    stage("Create environment") {
        env.STAGE_STATUS = "Creating conda environment"
        sh '''
            # Use the conda cache running on the Jenkins host
            conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
            conda config --set always_yes True
            conda create -n isis python=3
            conda env update -n isis -f environment.yml --prune
        '''
    }
    
    withEnv(isisEnv) {
        dir("build") {
            try {
                stage ("Build") {
                    env.STAGE_STATUS = "Building ISIS on ${env.OS}"
                    sh """
                        # Webhook test comment
                        source activate isis
                        cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                        ninja -j4 install
                    """
                }
            }
            catch(e) {
                build_ok = false
                println e.toString()
            }

            try{
                stage("AppTests") {
                    env.STAGE_STATUS = "Running app tests on ${env.OS}"
                    sh """
                        source activate isis
                        ctest -R _app_ -j4 -VV
                    """
                }
            }
            catch(e) {
                build_ok = false
                println e.toString()
            }

            try{
                stage("ModuleTests") {
                    env.STAGE_STATUS = "Running module tests on ${env.OS}"
                    sh """
                        source activate isis
                        ctest -j4 -VV -R _module_
                    """
                }
            }
            catch(e) {
                build_ok = false
                println e.toString()
            }

            try{
                stage("GTests") {
                    env.STAGE_STATUS = "Running gtests on ${env.OS}"
                    sh """
                        source activate isis
                        ctest -j4 -VV -R "." -E "(_app_|_unit_|_module_)"
                    """
                }
            }
            catch(e) {
                build_ok = false
                println e.toString()
            }
        }
        
        if(build_ok) {
            currentBuild.result = "SUCCESS"
        }
        else {
            currentBuild.result = "FAILURE"
        }
    }
}
