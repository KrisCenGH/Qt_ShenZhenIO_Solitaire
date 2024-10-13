#include "poke.h"

#include <QApplication>
#include <QMediaPlayer>
#include <QMediaPlaylist>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 准备音乐
    QMediaPlayer player;
    QMediaPlaylist playlist;

    playlist.addMedia(QUrl("qrc:/music/Solitaire.mp3")); // 原音频格式为.ogg，但是此Qt库暂不支持，故转换为.mp3
    playlist.setPlaybackMode(QMediaPlaylist::Loop); // 循环播放主背景音乐

    player.setPlaylist(&playlist);
    player.setVolume(80);
    player.play();

    Poke w;
    w.show();

    return a.exec();
}
