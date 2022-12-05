pipeline {

    agent {                                     //pick a build agent with label BLDLNX.
        label 'BLDLNX'                          //Sandbox can use BLDLNX/BLDIBM/BLDSUN,
                                                //GNRLD/GNRIBM/GNRSUN

    }
    options {
        disableConcurrentBuilds()               //this job never build concurrently
        buildDiscarder(logRotator(numToKeepStr: '15'))  //log rotation, keep 15 builds
        skipDefaultCheckout()                   //skips default checkout of this repo
        timeout(time: 10, unit: 'MINUTES')      //set timeout for this job
    }
    stages {                                    //stages
        stage('Print environment') {            //define a stage
             steps {
                             echo 'checking out important environments'
                             //tells you which user are you running this job as
                             //what is the current directory this job is running at
                             //on which host
                             sh '''
                             whoami
                             pwd
                             uname -a
                             '''
                  }
             }
        stage('Create or update phabricator'){
            when {
                branch "PR-*"               // a stage only runs for pull requests
            }
            steps{
                echo 'Running arc diff with --nolint (in case there is a bde_verify error)'
                sh """             
		        /opt/bb/bin/python3.8 /bb/bde/bbshr/bde-ci-tools/bin/phabricatorbot.py --verbose --nolint --create-checkout ${WORKSPACE} --url ${CHANGE_URL}
                """             
            }
        }
        stage('Update phabricator with lint'){
            when {
                branch "PR-*"               // a stage only runs for pull requests
            }
            steps{
                echo 'running arc diff on pull request (w/ lint)'
                sh """             
		        /opt/bb/bin/python3.8 /bb/bde/bbshr/bde-ci-tools/bin/phabricatorbot.py --verbose --create-checkout ${WORKSPACE} --url ${CHANGE_URL}
                """    
            }
        }
    }
    post {                                      //after the build, clean up
        always {
            echo 'cleaning up the workspace'
            deleteDir()
        }
        failure {                               //notify someone if the build fails
                    mail bcc: '',               //tweak from/to/subjects to use
                    cc: '',
                    replyTo: '',
                    from: 'bdebuild@jaas.dev.bloomberg.com',
                    to: env.CHANGE_AUTHOR + '@bloomberg.net',
                    subject: 'Build status of job :'+ env.JOB_NAME,
                    body: env.BUILD_URL + ' Failed!'
        }
    }
}
