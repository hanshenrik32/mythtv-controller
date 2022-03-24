pipeline {
  agent any
  stages {
    stage('Build') {
      agent { label 'mcenter' }
      steps {
        echo 'Building..'
        sh 'make compile'
      }
    }

    stage('Test') {
      agent { label 'mcenter' }
      steps {
        echo 'Testing..'
      }
    }

    stage('Deploy') {
      agent { label 'mcenter' }
      steps {
        echo 'Deploying....'
        sh 'make install'
      }
    }
  }
}