#!/bin/bash

flags=( "-c" )

rm report_body.tex

for file in `dir -d *.wav` ; do
	echo "\section*{Reporting file $file}" >> report_body.tex
	echo "\begin{center}" >> report_body.tex
	echo "\begin{tabular}{|l|l|l|l|l|}\hline" >> report_body.tex
	echo "flag & Inicial size[Kb] & Compressed size[Kb] & Compression ratio[s] & Checksum\\\\\\hline" >> report_body.tex
	for args in ${flags[@]}; do
		echo "Executing.. ./encoder $args $file compressed_file > /dev/null"
		./encoder -c $file compressed_file > /dev/null
		c_size=`ls -s --block-size=1 compressed_file  | cut -f1 -d' '`
		o_size=`ls -s --block-size=1 $file  | cut -f1 -d' '`
		compression_ratio=`echo "scale=4;$c_size / $o_size" | bc`
		echo "Executing.. ./decoder compressed_file $file.out > /dev/null"
		./decoder compressed_file $file.out > /dev/null
		a=`md5sum $file | cut -d' ' -f1`
		b=`md5sum $file.out | cut -d' ' -f1`
		if [ "$a" = "$b" ]
		then
			checksum="OK"
		else
			checksum="FAIL"
		fi
		echo "$args & $(($o_size/1024)) & $(($c_size/1024)) & $compression_ratio & $checksum \\\\\\hline" >> report_body.tex
	done
	echo "\end{tabular}" >> report_body.tex
	echo "\end{center}" >> report_body.tex
done

pdflatex report.tex

xpdf report.pdf || okular report.pdf || evince report.pdf
