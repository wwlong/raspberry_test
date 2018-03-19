alsamixer  #调节声音大小
##首先要安装mpg321
sudo apt-get -y install mpg321  #安装这个可以播放MP3
##准备播放列表
$ ls /Users/mcong/Music/*.mp3 > favorites.list

##后台播放


$ mpg321 -q --list favorites.list &

##关闭


$ fg

[1]  + 10268 running    mpg321 -q --list favorites.list

##wav
sudo apt-get install alsa-utils
sudo aplay /usr/share/sounds/alsa/Front_Center.wav #测试播放
sudo aplay test.wav #播放wav格式音乐

