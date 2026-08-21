pipeline {

    agent {
        label 'BDEBLL'                          //pick a build agent with label
    }
    options {
        disableConcurrentBuilds()               //this job never build concurrently
        buildDiscarder(logRotator(numToKeepStr: '15'))  //log rotation, keep 15 builds
        skipDefaultCheckout()                   //skips default checkout of this repo
        timeout(time: 120, unit: 'MINUTES')      //set timeout for this job
    }
    stages {                                    //stages
        stage('Print environment') {            //define a stage
             steps {
                 echo "checking out important environments "
                 //tells you which user are you running this job as
                 //what is the current directory this job is running at
                 //on which host
                 sh '''
                 whoami
                 pwd
                 uname -a
                 echo "=== === ENVIRONMENT === ==="
                 echo BUILD_ID="\"$BUILD_ID\""
                 echo BUILD_NUMBER="\"$BUILD_NUMBER\""
                 echo BUILD_TAG="\"$BUILD_TAG\""
                 echo BUILD_URL="\"$BUILD_URL\""
                 echo CHANGE_URL="\"$CHANGE_URL\""
                 echo EXECUTOR_NUMBER="\"$EXECUTOR_NUMBER\""
                 echo HOSTNAME="\"$HOSTNAME\""
                 echo JAVA_HOME="\"$JAVA_HOME\""
                 echo JENKINS_URL="\"$JENKINS_URL\""
                 echo JOB_NAME="\"$JOB_NAME\""
                 echo NODE_NAME="\"$NODE_NAME\""
                 echo PATH="\"$PATH\""
                 echo USER="\"$USER\""
                 echo WORKSPACE="\"$WORKSPACE\""
                 '''
            }
        }
        stage('Run BDE CI Bot'){
            when {
                branch "PR-*"               // a stage only runs for pull requests
            }
            steps {
                echo 'Running BDE CI Bot'
                sh '/opt/bb/bin/python3.8 /bb/bde/bbshr/bde-ci-tools/bin/bdecibot.py --verbose --nolint --url "$CHANGE_URL" --create-checkout "$WORKSPACE"'
            }
        }
        stage('Run BDE Branch Sanity Checks'){
            when {
                branch "PR-*"               // a stage only runs for pull requests
            }
            steps {
                echo 'Run branch validations'
                sh '/opt/bb/bin/bash /bb/bde/bbshr/bde-ci-tools/bin/bde_branch_check --cycle-check --pr-url "$CHANGE_URL"'
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
