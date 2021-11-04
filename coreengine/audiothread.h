#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QObject>
#include <QVector>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QIODevice>
#include <QTimer>

#include "coreengine/globalutils.h"
#include "3rd_party/oxygine-framework/oxygine-framework.h"

class AudioThread : public QObject
{
    Q_OBJECT
private:
    struct SoundData
    {
        static constexpr qint32 MAX_SAME_SOUNDS = 60;
        static constexpr qint32 DEFAULT_CACHE_SIZE = 10;
        std::shared_ptr<QSoundEffect> sound[MAX_SAME_SOUNDS];
        std::shared_ptr<QTimer> timer[MAX_SAME_SOUNDS];
        qint32 nextSoundToUse = 0;
    };
    struct Player
    {
        Player(QObject* parent)
            : m_player(parent)
        {
        }
        QMediaPlayer m_player;
        qint32 m_currentMedia{-1};
        qint32 m_nextMedia{-1};
    };
public:
    explicit AudioThread();
    virtual ~AudioThread();
    qint32 getSoundsBuffered();
    /**
     * @brief createSoundCache
     */
    void createSoundCache();
    // Qt Signals and Slots
signals:
    void sigInitAudio();
    void sigCreateSoundCache();
    void sigPlayMusic(qint32 File);
    void sigSetVolume(qint32 value);
    void sigAddMusic(QString File, qint64 startPointMs = -1, qint64 endPointMs = -1);
    void sigClearPlayList();
    void sigPlayRandom();
    void sigLoadFolder(QString folder);
    void sigPlaySound(QString file, qint32 loops, qint32 delay, float volume = 1.0f);
    void sigStopSound(QString file);
    void sigStopAllSounds();
    void sigChangeAudioDevice(const QVariant& value);
    void sigLoadNextAudioFile();
public slots:
    /**
     * @brief getLoadBaseGameFolders
     * @return
     */
    bool getLoadBaseGameFolders() const;
    /**
     * @brief setLoadBaseGameFolders
     * @param loadBaseGameFolders
     */
    void setLoadBaseGameFolders(bool loadBaseGameFolders);
    /**
     * @brief playMusic
     * @param File the music file to be played
     */
    void playMusic(qint32 File);
    /**
     * @brief addMusic
     * @param File adds a file to the playlist
     * @param startPointMs when this file is played the music will start at this position in ms. Values smaller 0 mean start at 0ms
     * @param endPointMs when this file is played the music will end at this position in ms and the next file will be played. Values smaller 0 mean play till end.
     */
    void addMusic(QString File, qint64 startPointMs = -1, qint64 endPointMs = -1);
    /**
     * @brief loadFolder loads all mp3 to the playlist
     * @param folder  the folder to be loaded
     */
    void loadFolder(QString folder);
    /**
     * @brief setVolume changes the volume between 0 and 100
     * @param value
     */
    void setVolume(qint32 value);
    /**
     * @brief getVolume
     * @return the current volume in percent 0 to 100
     */
    qint32 getVolume();
    /**
     * @brief clearPlayList clears the current playlist
     */
    void clearPlayList();
    /**
     * @brief playRandom plays a random mp3 from the playlist
     */
    void playRandom();
    /**
     * @brief playSound
     * @param file
     * @param loops
     */
    void playSound(QString file, qint32 loops = 1, qint32 delay = 0, float volume = 1.0f);
    /**
     * @brief stopSound
     * @param file
     */
    void stopSound(QString file);
    /**
     * @brief stopAllSound
     */
    void stopAllSounds();
    /**
     * @brief changeAudioDevice
     * @param value
     */
    void changeAudioDevice(const QVariant& value);
protected slots:
    // stops current Music and launches another one.
    void SlotPlayMusic(qint32 file);
    void SlotSetVolume(qint32 value);
    void SlotAddMusic(QString file, qint64 startPointMs = -1, qint64 endPointMs = -1);
    void SlotClearPlayList();
    void SlotPlayRandom();
    void SlotLoadFolder(QString folder);
    void SlotCheckMusicEnded(qint64 duration);
    // audio stuff
    void SlotPlaySound(QString file, qint32 loops,qint32 delay, float volume = 1.0f);
    void SlotStopSound(QString file);
    void SlotStopAllSounds();
    void initAudio();
    void SlotChangeAudioDevice(const QVariant& value);
    /**
     * @brief loadNextAudioFile
     * @param playerIndex
     */
    void loadNextAudioFile();
protected:
    /**
     * @brief createPlayer
     * @param player
     */
    void createPlayer();
    /**
     * @brief addMusicToTempFolder
     * @param file
     * @param startPointMs
     * @param endPointMs
     */
    void addMusicToPlaylist(QString file, qint64 startPointMs = -1, qint64 endPointMs = -1);
    /**
     * @brief clearTempFolder
     */
    void clearTempFolder();
    /**
     * @brief loadMusicFolder
     * @param folder
     * @param loadedSounds
     */
    void loadMusicFolder(QString folder, QStringList& loadedSounds);
    /**
     * @brief readSoundCacheFromXml
     * @param folder
     */
    void readSoundCacheFromXml(QString folder);
    /**
     * @brief fillCache
     * @param cache
     * @param count
     * @param file
     */
    void fillSoundCache(qint32 count, QString folder, QString file);
    /**
     * @brief reportReplayError
     * @param error
     * @param errorString
     */
    void reportReplayError(QMediaPlayer::Error error, const QString &errorString);
    /**
     * @brief mediaStatusChanged
     * @param status
     */
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    /**
     * @brief mediaPlaybackStateChanged
     * @param playerIndex
     * @param newState
     */
    void mediaPlaybackStateChanged(QMediaPlayer::PlaybackState newState);
    /**
     * @brief playSoundAtCachePosition
     * @param soundCache
     * @param i
     * @return
     */
    bool tryPlaySoundAtCachePosition(std::shared_ptr<SoundData> & soundCache, qint32 i,
                                     QString & file, qint32 loops, qint32 delay, qreal sound);
    /**
     * @brief stopSound
     * @param soundData
     * @param soundIndex
     */
    void stopSound(std::shared_ptr<SoundData> & soundData, qint32 soundIndex);
    void loadMediaForFile(QString filePath);
private:
    // music playback data
    struct PlaylistData
    {
        explicit PlaylistData(QString file, qint32 startpointMs = -1, qint32 endpointMs = -1)
            : m_startpointMs(startpointMs),
              m_endpointMs(endpointMs),
              m_file(file)
        {
        }
        qint32 m_startpointMs{-1};
        qint32 m_endpointMs{-1};
        QString m_file;
    };
    QTimer m_positionChangedTimer;
    QScopedPointer<Player> m_player;
    QVector<PlaylistData> m_PlayListdata;
    // sound playback data
    QMap<QString, std::shared_ptr<SoundData>> m_soundCaches;
    // general audio info
#ifdef AUDIOSUPPORT
    QAudioDevice m_audioDevice;
    QAudioOutput m_audioOutput;
#endif
    bool m_loadBaseGameFolders{true};
};

#endif // AUDIOTHREAD_H
