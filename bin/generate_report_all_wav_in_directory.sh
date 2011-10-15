for file in `dir -d *.wav` ; do
echo "###Generating report with file $file###"
./generate_report.sh "$file"
echo "##########################################"
done
