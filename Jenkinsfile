pipeline {
  agent any
  stages {
    stage('error') {
      steps {
        echo 'Build error'
      }
    }

  }
  stages('compile') {
   steps {
    echo 'compile compile'
    sh 'make compile'
    sh 'make install'
    //mail bcc: '', body: 'Build ok', cc: '', from: 'hanshenrik32@gmail.com', replyTo: '', subject: 'mythtv-controller build ok', to: 'hanshenrik32@gmail.com'
     
   }
  }
}