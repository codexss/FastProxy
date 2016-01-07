if [ "$mode" == "" ]; then
mkdir FastProxy >/dev/null 2>/dev/null
for file in ./*.conf
do
	if test -f $file
	then
		if [ $file != "$0" ]
		then
			rm -f ./FastProxy/${file%.*}.fp
			cat $file $0 | bash >./FastProxy/${file%.*}.fp 2>/dev/null
		fi
	fi
done
exit
fi

echo "# 本模式由TinyToFP自动生成"
echo
echo

echo 设置 HTTP代理 $http_ip:$http_port
echo 设置 HTTPS代理 $https_ip:$https_port

echo

echo 模块 HTTP
if [ "$http_del" != "" ]; then
echo 删除 "$http_del" >tmp
sed -e 's/,/ /g' tmp
rm -rf tmp
fi
if [ "$http_first" != "" ]; then
echo 设置首行 "$http_first"
fi

echo

echo 模块 HTTPS
if [ "$https_del" != "" ]; then
echo 删除 "$https_del" >tmp
sed -e 's/,/ /g' tmp
rm -rf tmp
fi

if [ "$https_first" != "" ]; then
echo 设置首行 "$https_first"
fi
