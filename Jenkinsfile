pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building..'
		sh 'make compile'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
		sh 'make install'
		//mail bcc: '', body: 'Build ok', cc: '', from: 'hanshenrik32@gmail.com', replyTo: '', subject: 'mythtv-controller build ok', to: 'hanshenrik32@gmail.com
            }
        }
    }
}