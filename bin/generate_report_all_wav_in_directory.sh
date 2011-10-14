for file in `dir -d *.wav` ; do
./generate_report.sh "$file"
done
