echo "#### STARTING ENCODER ####"
./encoder -c $1 compressed_file > encoder.$1.out
c_size=`ls -s --block-size=1 compressed_file  | cut -f1 -d' '`
o_size=`ls -s --block-size=1 $1  | cut -f1 -d' '`
compression_ratio=`echo "scale=4;$c_size / $o_size" | bc`
echo "#### STARTING DECODER ####"
./decoder compressed_file $1.out > decoder.$1.out
a=`md5sum $1 | cut -d' ' -f1`
b=`md5sum $1.out | cut -d' ' -f1`
if [ "$a" = "$b" ]
then
	echo "CHECKSUM OK -> Original file size=$o_size, Compressed file size=$c_size, Compression ratio=$compression_ratio"
else
	echo "CHECKSUM FAIL"
fi

