// vim: ft=groovy

def isisDataPath = '/isisData/data'
def isisTestDataPath = "/isisData/testData"
def isisBuildCache = "/isisBuild"

def isisEnv = [
    "ISIS3DATA=${isisDataPath}",
    "ISIS3TESTDATA=${isisTestDataPath}",
    "ISISROOT=${isisBuildCache}"
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
        cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
        isisEnv.add("ISIS_SRC_DIR=${pwd()}")
    }

    stage("Create environment") {
        env.STAGE_STATUS = "Creating conda environment"
        sh '''
            # Use the conda cache running on the Jenkins host
            conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
            conda config --set always_yes True
            conda create -n isis python=3
        '''

        if (env.OS.toLowerCase() == "centos") {
            sh 'conda env update -n isis -f environment_gcc4.yml --prune'
        } else {
            sh 'conda env update -n isis -f environment.yml --prune'
        }
    }
    
    withEnv(isisEnv) {
        dir("${env.ISISROOT}") {
            stage ("Build") {
                env.STAGE_STATUS = "Building ISIS on ${env.OS}"
                sh """
                    source activate isis
                    cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                    ninja -j4 install
                    python ${env.ISIS_SRC_DIR}/isis/scripts/isis3VarInit.py --data-dir ${env.ISIS3DATA} --test-dir ${env.ISIS3TESTDATA}
                """
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
                        ctest -R _module_ -j4 -VV
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
                        ctest -R "." -E "(_app_|_unit_|_module_)" -j4 -VV
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
