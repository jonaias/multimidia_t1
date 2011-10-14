echo "#### STARTING ENCODER ####"
./encoder -c $1 compressed_file
echo "#### STARTING DECODER ####"
./decoder compressed_file $1.out
md5sum $1
md5sum $1.out