#!/bin/bash
name="root"
pwd="zhouqin"
read -p "请输入数据库用户名：" username
read -s -p "请输入数据库密码：" password;echo
read -p "请输入备份起始时间:如：\"2018-03-15 08:30:35\" " starttime
read -p "请输入截止时间：" endtime
read -p "请输入数据库名：" sqlname

#scmd="use test;"SOE->t_yx_changed_yyyy_mm;+_yyyy_mm

#name="real_data_yc_`date -d $time +%Y_%m`"

starty=`date -d "$starttime" +%Y`
endy=`date -d "$endtime" +%Y`
startm=`date -d "$starttime" +%m`
endm=`date -d "$endtime" +%m`

if [ $endy == $starty ]
then
mondiff=$[$endm-$startm]
else 
mondiff=$[12*$[$endy-$starty]+$[$endm-$startm]]
fi

if [ $sqlname = "soe" ]
then
sqlname="t_yx_changed_"
else
sqlname=$sqlname"_"
fi

i=0
while(( i<=mondiff ))
do
time=`date -d "$starttime+$i month" +%Y_%m`
name=$sqlname$time

if [ $i == $mondiff ]
then 
mysql -u$username -p$password -e "use test;
create table if not exists $name like real_data_yc;
insert into $name select* from real_data_yc where datetime<'$endtime'
and (month(datetime)-month('$starttime'))=$i;
exit"
break
fi

mysql -u$username -p$password -e "use test;
create table if not exists $name like real_data_yc;
insert into $name select* from real_data_yc where datetime>'$starttime'
and (month(datetime)-month('$starttime'))=$i;
exit"
let "i++"
done
