#!/bin/bash
date | tee --append log.txt
../Debug/dedup4 sync
echo "Note - Path of all the file name you enter is relative to the location of this script."
echo "Note - The output for is saved to log.txt." 
while true; do
echo
echo
echo "What test do you want to run ?"
echo "1.  Write a new file"
echo "2.  Read a file"
echo "3.  Write a file using Content Aware Chunking"
echo "4.  Flush the file system"
echo
echo

echo -n "Enter your choice, or 0 for exit: "
read choice
echo

case $choice in
     1)
     echo "Enter the name of file you want to write(using fixed size chunking) or press 0 for default file:"
     read filename1
     case $filename1 in
          0)
	   ../Debug/dedup4 store test.fasta | tee --append log.txt
     	   ;;
          *)
	  ../Debug/dedup4 store $filename1 | tee --append log.txt
	   ;;
     esac
     ;;  	
     2)
     echo "Enter the name of file you want to read(the test file is named test.fasta):"
     read filename2
     echo "Enter the name of file, you want to dump the data from file read or press 0 for default name(dump_new.fasta):"
     read filename3
     case $filename3 in
 	  0)
	  ../Debug/dedup4 query $filename2 dump_new.fasta | tee --append log.txt
	  ;;
	  *)
	  ../Debug/dedup4 query $filename2 $filename3 | tee --append log.txt
     esac
     ;;
     3)
     echo "Enter the name of file you want to write(using content aware chunking) or press 0 for default file: "
     read filename4
     case $filename4 in
          0)
	   ../Debug/dedup4 cac test.fasta | tee --append log.txt
     	   ;;
          *)
	  ../Debug/dedup4 cac $filename4 | tee --append log.txt
	   ;;
     esac	
	rm ofl*
     ;;
     4)
     echo "Flushing the file system."
     ../Debug/dedup4 sync | tee --append log.txt
     ;;
     0)
     echo "Thanks!!"
     break
     ;;
     *)
     echo "That is not a valid choice, try a number from 0 to 3."
     ;;
esac  
done
