README
=============

Our project is not a portion of JOS and hence it works only on Linux. 

The project folder also contains a default file system where we store the data. 

Use '/ts/testscript.sh' to run all the test cases. The file system works for Genome data which constitutes of characters 'A', 'G', 'C' and 'T'. The fasta file should only contain the sequence without any whitespace characters.

The test data set that we are using are stored in '/ts' and is named as: 
E.coli U00096.1 – ecoli1_w.fasta
E.coli U00096.2 – ecoli2_w.fasta
E.coli U00096.3 – ecoli3_w.fasta 

We are creating intermediate files while reading a file in case of content aware chunking to check if read is proper. 



