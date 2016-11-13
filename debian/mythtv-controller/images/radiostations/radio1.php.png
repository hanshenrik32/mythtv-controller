<?PHP     
/*Simple, good looking recursive function for printing directories.
Just copy/paste and it is ready to go!*/

function printCurrentDirRecursively($originDirectory, $printDistance=0){
   
    // just a little html-styling
    if($printDistance==0)echo '';
    $leftWhiteSpace = "";
    for ($i=0; $i < $printDistance; $i++)  $leftWhiteSpace = $leftWhiteSpace."&nbsp;";
   
    $nr=0;   
    $CurrentWorkingDirectory = dir($originDirectory);
    while($entry=$CurrentWorkingDirectory->read()){
        if($entry != "." && $entry != "..") {
            
            echo "nr = " . $nr . "\n";
            
            if(is_dir($originDirectory."\\".$entry)){
                // echo $leftWhiteSpace."".$entry."n";
                printCurrentDirRecursively($originDirectory."\\".$entry, $printDistance+2);
             }
            else {
                $handle=fopen("/home/hans/tunein.com/radio/" . $entry. "/index.html","r");
                if ($handle!=0) {
                    $fil=fread($handle,filesize("/home/hans/tunein.com/radio/" . $entry. "/index.html"));
                    $pos=strpos($fil,"<meta property=\"og:image\" content=\"");

                    
                    //$namepos=strpos($fil,"<meta property=\"og:title\" content=\"");
                    //$name=substr($fil,$namepos,120); 
                    //$nameend=strpos($fil,"/>");                    
                    //$name=substr($name,0,$nameend);
                    
                    if ($pos) {
                        $fil=substr($fil,$pos,120);
                        $pos=strpos($fil," />");
                        //print substr($fil,0,$pos) . "\n ";
                        
                        $nyline=substr($fil,25+9,$pos);
                        
                        $pos=strpos($nyline," />");
                        
                        
                        $namepos=strrpos($entry,"-",-1);
                        
                        $name=substr($entry,0,$namepos);
                        
                        
                        print "NamE:" . $name. " " . substr($nyline,0,$pos) . "\n";
                        
                        //system("wget " . substr($nyline,0,$pos) . " -O " . $name);                        
                        
                    }                    
                }
                fclose($handle);
                //echo $leftWhiteSpace.$entry."\n";
            }
            $nr=$nr+1;
        }
    }
    $CurrentWorkingDirectory->close();
   
    if($printDistance==0)echo "";
}

//TEST IT!
printCurrentDirRecursively("/home/hans/tunein.com/radio/");

?>

