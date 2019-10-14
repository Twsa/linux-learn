#!/usr/bin/env bash
setup_content=$HOME/Desktop/my_test/qcd
if [ ! -d $setup_content ]; then
  mkdir $setup_content
fi
qcd_dir=$setup_content/qcd.txt
if [ ! -e "$qcd" ]; then
    touch $qcd_dir
fi
help()
{
    echo "-h help"
    echo "-a add directory"
    echo "-l list all of directories!"
    echo "-r remove directory through "
    echo "-c clear directories"
}

#列出某个文件的行号
list_num ()
{
  FILENAME=$1
  if [ $# -ne 1 ];then
     echo "USEAGE: list_num arg ,arg is a chanchu file name"
     return 1
  fi
  LOOP=1
  while read LINE
  do
  echo "$LOOP:  $LINE"
  LOOP=`expr $LOOP + 1`
  done <$FILENAME
}
add()
{
    if [ $1 ] && [ -d $1 ] ; then
        line_num=`cat $qcd_dir | wc -l`
        new_num=`expr $line_num + 1`
        echo  $1 >> $qcd_dir
        echo "Add successfully!"
    else
        if [ $1 ];then
            echo "\"$1\" directory don't exist!"
        else
            help
        fi
    fi
}

#列出所有的已经添加的目录
list()
{
    list_num $qcd_dir
}

#删除特定的行
remove()
{
    # echo  "$new_num:$1" >> $qcd_dir
    list_num $qcd_dir
    line_num=`cat $qcd_dir | wc -l`
    echo  "1-$line_num,please enter..."
    read num_rm
    # echo `echo $num_rm`d
    sed -i `echo $num_rm`d $qcd_dir
    # for((i=$num_rm;i<=$line_num;i++));
    # do 
    #         echo  "$new_num:$1" >> $qcd_dir
    # done
}

#进入特定的目录
enter()
{
    list_num $qcd_dir
    line_num=`cat $qcd_dir | wc -l`
    echo  "1-$line_num,please enter..."
    read num_en
    # echo `echo $num_rm`d
    tmp="$(sed -n  `echo $num_en`p  $qcd_dir)"
    cd $tmp
}

#清空所有的目录
clear()
{
    if [ -e $qcd_dir ];then
        echo -e "$qcd_dir exit,\nreally clear it? [Y/n]: " 
        read  REPLACE
        case $REPLACE in
            [yY][eE][sS]|[yY])
            REPLACE="yes"
            echo "$qcd_dir have removed."
            >$qcd_dir
            ;;
            [nN][oO]|[nN])
            REPLACE="no"
            echo "do nothing!"
            ;;
            *)
            echo "Input error, should be [Y/n]."
            exit 1
        esac    
    fi
    #rm $qcd_dir
}
#一些事件的处理过程
case $1 in
-a)
    add $2
    ;;
-r)
    remove $2
    ;;
-l)
    list  
    ;;
-c)
    clear 
    ;;
-e)
    enter
    ;;
*)
    help 
    ;;
esac
