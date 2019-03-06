#!/bin/bash
read -p "请输入文件名：" filename
#read -s -p "请输入数据库密码：" password;echo
read -p "请输入生成几天数据：" day
read -p "请输入数据最后生成时间:如：2018-03-15 08:30:35 " mtime
#read -p "请输入数据库名：" sqlname
filenum=$[24*60*$day]
suffix="."${filename#*"."}
echo $suffix
#mtime=`date -d "$mtime 1 minute ago" '+%Y-%m-%d %T'`
#echo $mtime
i=0
while(( $i<=$filenum ))
do
mtime=`date -d "$mtime 1 minute ago" '+%Y-%m-%d %T'`
name=`date -d "$mtime" +%Y%m%d%H%M%S`"00100"$suffix
echo $name
cp ./$filename ./$name
touch -m -d "$mtime" $name
let "i++"
done
