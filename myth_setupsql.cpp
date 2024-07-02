//
// Setup/create default stream db
//
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "myth_setupsql.h"

extern char *dbname;                                            // internal database name in mysql (music,movie,radio)

extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];


const char *create_sql_radiostations="CREATE TABLE IF NOT EXISTS `radio_stations` (`name` varchar(80) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`beskriv` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`stream_url` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`homepage` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`gfx_link` varchar(200) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,`art` int(4) NOT NULL,`bitrate` int(11) NOT NULL,`popular` int(11) NOT NULL,`aktiv` int(1) NOT NULL,`createdate` date NOT NULL, `intnr` int(11) NOT NULL AUTO_INCREMENT,`landekode` int(11) NOT NULL, `lastplayed` datetime DEFAULT NULL,`online` tinyint(1) NOT NULL DEFAULT '1', PRIMARY KEY (`intnr`), KEY `art` (`art`), KEY `aktiv` (`aktiv`), KEY `popular` (`popular`), KEY `name` (`name`)) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='rss radio stations'";
const char *create_sql_radio_countries="CREATE TABLE IF NOT EXISTS `radio_countries` (`land` varchar(80) NOT NULL,`landekode` int(11) NOT NULL,`imgpath` varchar(80) NOT NULL,PRIMARY KEY (`landekode`)) ENGINE=MyISAM DEFAULT CHARSET=utf8";
const char *create_sql_radio_types="CREATE TABLE IF NOT EXISTS `radiotypes` (`typename` varchar(80) NOT NULL, `art` int(2) NOT NULL, `sort` int(11) NOT NULL, PRIMARY KEY (`art`), KEY `sort` (`sort`)) ENGINE=MyISAM DEFAULT CHARSET=utf8";
const char *create_sql_radio_types_data="REPLACE INTO `radiotypes` (`typename`, `art`, `sort`) VALUES \
                                                                                    ('Rock/Classic Rock',  1,  5), \
                                                                                    ('Blues',              2,  2), \
                                                                                    ('All',                0,  0), \
                                                                                    ('Top 40',            40,  0), \
                                                                                    ('Dance/Techno',       4,  0), \
                                                                                    ('Jazz',               9,  0), \
                                                                                    ('Classical',          8,  5), \
                                                                                    ('Alternative',        7,  0), \
                                                                                    ('Country music',      6, 13), \
                                                                                    ('R&B/HipHop',         5,  7), \
                                                                                    ('Oldies',             3, 14), \
                                                                                    ('Trance',            41, 41), \
                                                                                    ('70''s music',       12, 15), \
                                                                                    ('80''s music',       13, 16), \
                                                                                    ('90''s music',       14, 17), \
                                                                                    ('HipHop',            15,  4), \
                                                                                    ('House',             16,  1), \
                                                                                    ('Most listened.',    19, 30), \
                                                                                    ('By artist.',        21, 31), \
                                                                                    ('Countries.',        27, 33), \
                                                                                    ('Bit rate.',         28, 34), \
                                                                                    ('Disco',             31, 36), \
                                                                                    ('Talk',              42, 42), \
                                                                                    ('Metal',             43, 43), \
                                                                                    ('Rap',               44, 44), \
                                                                                    ('Pop',               45, 45), \
                                                                                    ('Various',           46, 46), \
                                                                                    ('Funk',              47, 47), \
                                                                                    ('Reggae',            48, 48), \
                                                                                    ('Gospel',            49, 49), \
                                                                                    ('Techno',            50, 50), \
                                                                                    ('Folk',              51, 51), \
                                                                                    ('Mixed',             52, 52), \
                                                                                    ('Rhythm & Blues',    53, 53), \
                                                                                    ('Love songs',        54, 54), \
                                                                                    ('Electronic',        30, 30), \
                                                                                    ('Trance',            29, 29), \
                                                                                    ('Favorites first',   0, 0)";


const char *create_sql_lande_db="REPLACE INTO `radio_countries` (`land`, `landekode`, `imgpath`) VALUES \
                                                                ('Yugoslavia', 3, 'yu.png'), \
                                                                ('France',     4, 'fr.png'), \
                                                                ('Luxembourg', 5, 'luxembourg.png'), \
                                                                ('Holland',    6, 'nl.png'), \
                                                                ('Usa',        7, 'usa.png'), \
                                                                ('Tyskland',   8, 'de.png'), \
                                                                ('England',    9, 'uk.png'), \
                                                                ('Rusland',   10, 'ru.png'), \
                                                                ('Israel',    11, 'Israel.png'), \
                                                                ('Austria',   12, 'Austria.png'), \
                                                                ('Lebanon',   13, 'lebanon.png'), \
                                                                ('Latvia',    14, 'latvia.png'), \
                                                                ('Vietnam',   16, 'Vietnam.png'), \
                                                                ('Saudi-Arabia', 17, 'Saudi-Arabia.png'), \
                                                                ('as',        18, 'as.png'), \
                                                                ('brazil',    19, 'brazil.png'), \
                                                                ('Norge',     21, 'no.png'), \
                                                                ('Polen',     22, 'pl.png'), \
                                                                ('Sverige',   23, 'se.png'), \
                                                                ('Switzerland', 24, 'sw.png'), \
                                                                ('Mexico',    25, 'mexico.png'), \
                                                                ('Belgium',   26, 'be.png'), \
                                                                ('Canada',    27, 'ca.png'), \
                                                                ('Australia', 28, 'as.png'), \
                                                                ('Rusland',   29, 'ru.png'), \
                                                                ('Spain',     30, 'sp.png'), \
                                                                ('United Arab Emirates', 31, 'ae.png'), \
                                                                ('Hungary',   32, 'hu.png'), \
                                                                ('Thailand',  33, 'th.png'), \
                                                                ('Greece',    34, 'gr.png'), \
                                                                ('Bosnia and Herzegovina', 35, 'bk.png'), \
                                                                ('Bulgaria',  36, 'nu.png'), \
                                                                ('India',     37, 'in.png'), \
                                                                ('Portugal',  38, 'po.png'), \
                                                                ('Iran',      39, 'ir.png'), \
                                                                ('Korea',     40, 'ks.png'), \
                                                                ('Romania',   41, 'romania.png'), \
                                                                ('Italy',     42, 'it.png'), \
                                                                ('Finland',   43, 'fi.png'), \
                                                                ('Bahrain',   44, 'bahrain.png'), \
                                                                ('Denmark',   45, 'dk.png'), \
                                                                ('Chile',     46, 'chile.png'), \
                                                                ('Chile',     47, 'chile.png'), \
                                                                ('Slovakia',  48, 'Slovakia.png'), \
                                                                ('Ukraine',   49, 'Ukraine.png'), \
                                                                ('Hungary',   50, 'hu.png'), \
                                                                ('Colombia',  51, 'co.png'), \
                                                                ('Dominican Republic', 52, 'do.png'), \
                                                                ('Azerbaijan',53, 'Azerbaijan.png'), \
                                                                ('Lithuania', 54, 'Lithuania.png'), \
                                                                ('Andorra',   55, 'Andorra.png'), \
                                                                ('Estonia',   56, 'Estonia.png'), \
                                                                ('Tajikistan',57, 'Tajikistan.png'), \
                                                                ('Turkey',    58, 'Turkey.png'), \
                                                                ('Mongolia',  59, 'Mongolia.png'), \
                                                                ('Belarus',   60, 'Belarus.png'), \
                                                                ('Slovenia',  61, 'Slovenia.png'), \
                                                                ('Cyprus',    62, 'Cyprus.png'), \
                                                                ('China',     63, 'China.png'), \
                                                                ('Cambodia',  64, 'Cambodia.png'), \
                                                                ('Indonesia', 65, 'Indonesia.png'), \
                                                                ('Singapore', 66, 'Singapore.png'), \
                                                                ('Croatia',   67, 'Croatia.png'), \
                                                                ('Czech Republic', 68, 'Czech Republic.png')";

//
// select to db the start
// select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by intnr
//
// \xE6 = æ
// \xF8 = ø
// All radiostations updated OK 07/11/2023


//
// CREATE TABLE IF NOT EXISTS `radio_stations`  where is the table def.
// (`name` , `beskriv`,`stream_url`,`homepage` , `gfx_link` ,`art` , `bitrate` ,`popular` ,`aktiv` ,`createdate` , `intnr` ,`landekode` , `lastplayed` ,`online` 
//


const char *radio_station_setupsql="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('P1 DK', 'Danish radio play clasisk music [Bitrate: 128]', 'http://live-icy.gss.dr.dk:8000/A/A03H.mp3', '', 'DR-P1-908.png', 8, 320, 63, 1, '2011-07-29', 1, 45, '2011-09-16 21:50:41', 1), \
('P2 DK', 'Dansk radio', 'http://live-icy.gss.dr.dk:8000/A/A04H.mp3', 'www.dr.dk/P2', 'DR-P2-1023.png', 1, 320, 62, 1, '2011-07-28', 2, 45, '2011-09-15 22:20:34', 1), \
('P3 DK', 'www.p3.dk Dansk radio.', 'http://live-icy.gss.dr.dk:8000/A/A05H.mp3', 'www.dr.dk', 'DR-P3-939.png', 1, 128, 77, 1, '2011-07-28', 3, 45, '2011-10-20 15:37:44', 1), \
('P4 K0benhavn', '', 'http://live-icy.gss.dr.dk:8000/A/A08H.mp3', 'http://www.dr.dk/p4/kbh/', 'DR-P4-Koebenhavn-965.png', 0, 0, 0, 1, '2011-11-11', 17, 0, '2017-01-01 00:00:00', 1), \
('P4 Esbjerg', '', 'http://live-icy.gss.dr.dk:8000/A/A15H.mp3', 'http://www.dr.dk/Regioner/Esbjerg/', 'DR-P4-Esbjerg-990.png', 0, 0, 0, 1, '2011-11-11', 18, 0, '2017-01-01 00:00:00', 1), \
('P6 Beat', '', 'http://live-icy.gss.dr.dk:8000/A/A29H.mp3', 'http://www.dr.dk/p6beat/', 'DR-P6-Beat.png', 0, 0, 0, 1, '2011-11-11', 19, 0, '2017-01-01 00:00:00', 1), \
('P7 Mix DK', '[Bitrate: 128]', 'http://live-icy.gss.dr.dk:8000/A/A21H.mp3', '', 'DR-P7-Mix.png', 40, 320, 64, 1, '2011-07-30', 20, 45, '2011-09-29 13:40:49', 1), \
('MAXXIMA', '', 'https://maxxima.mine.nu/maxxima.mp3', '', 'Maxxima.png', 4, 0, 10, 1, '2011-08-02', 28, 4, '2011-10-02 02:25:35', 1), \
('Bay Radio', '[Bitrate: 128]', 'https://listen-nation.sharp-stream.com/tcbay.mp3', 'http://www.swanseabayradio.com/', '', 45, 320, 1, 1, '2011-07-31', 29, 45, '2011-09-17 18:06:22', 1), \
('181FM', '', 'http://uplink.181.fm:8068/', '', '181.fm.png', 0, 128, 1, 1, '2011-08-02', 31, 7, '2011-09-27 17:58:45', 1), \
('Energy 93 Eurodance', '', 'http://uplink.181.fm:8044/', '', '181.fm.png', 4, 0, 5, 1, '2011-08-02', 32, 7, '2011-10-07 16:02:00', 1), \
('Just Hiphop', '', 'http://stream.laut.fm:80/justhiphop', '', '', 5, 0, 2, 1, '2011-08-02', 33, 29, '2011-09-13 10:53:29', 1), \
('Radio EFM', 'France', 'http://radioefm.ice.infomaniak.ch:80/radioefm-high.mp3', '', '', 12, 128, 6, 1, '2011-08-01', 36, 4, NULL, 1), \
('Capital 95.8', '', 'http://media-ice.musicradio.com:80/CapitalMP3', '', '', 40, 192, 2, 1, '2011-08-05', 39, 9, NULL, 1), \
('TechnoBase.FM', '', 'http://listen.technobase.fm/tunein-dsl-pls', 'http://TechnoBase.FM', '', 50, 192, 1, 0, '2011-08-05', 12, 8, NULL, 1), \
('Slow Radio', '(Love Songs & Easy Listening)', 'http://stream2.slowradio.com', 'http://en.slowradio.com/', 'slowradio.png', 54, 192, 1, 1, '2013-10-13', 51, 4, NULL, 1), \
('Radio88 Szeged FM 95.4', '[Bitrate: 192]', 'http://88.151.99.22:8000', '', 'radio88.png', 30, 192, 0, 1, '2023-10-13', 74, 32, NULL, 1), \
('BABELSBERG HITRADIO', '[Bitrate: 192]', 'http://46.163.75.84:8000', '', 'radiobheins.png', 40, 192, 1, 1, '2023-10-13', 171, 42, NULL, 1), \
('Radio Stand By', '[Bitrate: 192]', 'http://188.165.206.81:8010', '', '', 46, 192, 0, 1, '2011-08-10', 422, 33, NULL, 1), \
('RadioZapliN', '[Bitrate: 192]', 'http://91.209.128.139:8000', '', '', 46, 192, 0, 1, '2011-08-10', 454, 29, NULL, 1), \
('Proxima FM', '[Bitrate: 192]', 'http://91.187.93.115:8000', '', 'proxima-fm.png', 46, 192, 0, 1, '2011-08-10', 557, 55, NULL, 1), \
('GRIMALDIFM', '[Bitrate: 192]', 'http://91.121.29.128:8058', '', 'grimaldifm.png GRIMALDIFM', 46, 192, 0, 1, '2011-08-10', 603, 4, NULL, 1), \
('Pure Jazz Radio', '', 'http://71.125.12.37:8000', '', '', 9, 128, 0, 1, '2011-08-11', 1064, 7, NULL, 1), \
('1980s.FM', '[Bitrate: 128]', 'http://209.9.229.214:80', '', '1980s-fm.png', 46, 128, 0, 1, '2011-08-11', 1086, 7, NULL, 1), \
('ANDYS 80S', '[Bitrate: 128]', 'http://209.9.238.4:9844', '', 'andys80s.png', 46, 128, 0, 1, '2011-08-11', 1105, 7, NULL, 1), \
('SOULPOWERfm', '101-va-motown_summer_mix  [Bitrate: 128]  ', 'http://91.143.83.37:12000', '', '', 0, 128, 0, 1, '2011-08-11', 1153, 8, NULL, 1), \
('RadioMusic', '[Bitrate: 128]', 'http://188.165.206.81:8110', '', '', 0, 128, 0, 1, '2011-08-11', 1166, 33, NULL, 1), \
('RadioGadangme', '[Bitrate: 128]', 'http://38.96.148.138:8670', '', '', 0, 128, 0, 1, '2011-08-11', 1182, 7, NULL, 1), \
('Rewound Radio', '[Bitrate: 128]', 'http://71.125.37.66:9000', '', 'rewound-radio.png', 3, 128, 0, 1, '2011-08-11', 1192, 0, NULL, 1), \
('TOP FM Beograd 106', '', 'http://109.206.96.12:8000', '', '', 46, 128, 0, 1, '2011-08-11', 1197, 8, NULL, 1), \
('Radio 80', '[Bitrate: 128]', 'http://94.23.2.73:7020', '', '', 13, 128, 0, 1, '2011-08-11', 1232, 4, NULL, 1), \
('Radio Twilight', 'Live vanuit Heusden met DJ Jo en Coba Flamingo  [Bitrate: 128]  ', 'http://81.18.165.234:8000', 'www.radio-twilight.nl', '', 0, 128, 0, 1, '2011-08-11', 1282, 6, NULL, 1), \
('A State Of Trance', '[Bitrate: 128]', 'http://89.252.2.4:8000', '', '', 41, 128, 1, 1, '2011-08-11', 1300, 49, NULL, 1), \
('Radio GARDARICA', 'Russia S-Petersburg', 'http://195.182.132.18:8260', '', '', 0, 128, 0, 1, '2011-08-11', 1316, 29, NULL, 1), \
('Memoryradio 2', '[Bitrate: 128]', 'http://188.165.247.175:5000', '', '', 0, 128, 0, 1, '2011-08-11', 1331, 4, NULL, 1), \
('KCSN Classical', ' Northridge ', 'http://130.166.82.14:8002', '', '', 8, 128, 0, 1, '2011-08-11', 1347, 7, NULL, 1), \
('Radio Kosova 91.9', '[Bitrate: 128]', 'http://82.114.72.2:8088', '', 'radio-kosova.png', 46, 128, 0, 1, '2011-08-11', 1485, 7, NULL, 1), \
('RadioSals4te', '[Bitrate: 128]', 'http://188.165.206.81:8100', '', '', 46, 128, 0, 1, '2011-08-11', 1689, 33, NULL, 1), \
('Radio Limfjord', '[Bitrate: 128]', 'http://media.wlmm.dk/limfjord', '', '', 46, 128, 0, 1, '2017-06-10', 1711, 45, NULL, 1);";


const char *radio_station_setupsql4="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Split Infinity', '[Bitrate: 128]', 'http://46.4.120.170:80', '', '', 45, 128, 0, 1, '2011-08-11', 1789, 8, NULL, 1), \
('EBM Radio', 'Invincible Limit', 'http://87.106.138.241:7000', '', '', 0, 128, 0, 1, '2011-08-11', 1820, 7, NULL, 1), \
('4U Classic Rock', 'Classic Rock Station  [Bitrate: 128]  ', 'http://217.114.200.100:80', '', '', 0, 128, 0, 1, '2011-08-11', 1964, 29, NULL, 1), \
('DJFM Ukraine', '[Bitrate: 128]', 'http://217.20.164.163:8010', '', '', 0, 128, 0, 1, '2011-08-11', 1974, 49, NULL, 1), \
('NAXI RADIO 96', '9MHz - Beograd - www.naxi.rs ', 'http://193.243.169.34:9150', '', '', 0, 128, 0, 1, '2011-08-11', 2009, 45, NULL, 1), \
('Electro-Radio', '24h nonStop Electro House Music', 'http://stream.electroradio.ch:26630/', 'http://www.electroradio.ch/', 'Electro-Radio.png', 30, 0, 0, 1, '2011-11-11', 2564, 0, '2017-01-01 00:00:00', 1), \
('Captain--Tennille', '', 'http://cp2.digi-stream.com:9094', '', 'Captain--Tennille.png', 0, 0, 0, 1, '2011-11-11', 2598, 0, '2017-01-01 00:00:00', 1), \
('Radio-Nostalgia', '', 'http://listen.radionomy.com/radio-nostalgia', 'http://www.radio-nostalgia.nl/', 'Radio-Nostalgia.png', 0, 0, 0, 1, '2011-11-11', 2605, 0, '2017-01-01 00:00:00', 1), \
('TEEZ-FM', '', 'http://listen.radionomy.com/teez-fm', 'http://teez-fm.blogspot.com/', 'TEEZ-FM.png', 0, 0, 0, 1, '2011-11-11', 2673, 0, '2017-01-01 00:00:00', 1), \
('KQNA-1130', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.kqna.com/', 'KQNA-1130.png', 0, 0, 0, 1, '2011-11-11', 2757, 0, '2017-01-01 00:00:00', 1), \
('XL93-929', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.xl93.com/', 'XL93-929.png', 0, 0, 0, 1, '2011-11-11', 2758, 0, '2017-01-01 00:00:00', 1), \
('FM-Music-f-Life', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.1077musicforlife.com/', '1077-FM-Music-For-Life.png', 0, 0, 0, 1, '2011-11-11', 2759, 0, '2017-01-01 00:00:00', 1), \
('ESPN-1240', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.star94radio.com/ESPN1240.php', 'ESPN-1240.png', 0, 0, 0, 1, '2011-11-11', 2760, 0, '2017-01-01 00:00:00', 1), \
('Selena-Gomez--The-Scene', '', 'http://stream.amellus.com:8000/KQNA.mp3', '', 'Selena-Gomez--The-Scene.png', 0, 0, 0, 1, '2011-11-11', 2761, 0, '2017-01-01 00:00:00', 1), \
('Chet-Baker', '', 'http://stream.amellus.com:8000/KQNA.mp3', '', 'Chet-Baker.png', 0, 0, 0, 1, '2011-11-11', 2762, 0, '2017-01-01 00:00:00', 1), \
('Radio-Amore-i-migliori-anni-Catania-916', 'Radio Amore i migliori anni non &#232; una semplice radio ma un vero e proprio museo della musica anni &#39;60, &#39;70, &#39;80 curato da professionisti del settore e appassionati del genere. Il gran', 'http://onair11.xdevel.com:8032', 'http://www.grupporadioamore.it/', 'Radio-Amore-i-migliori-anni-Catania-916.png', 0, 0, 0, 1, '2011-11-11', 2796, 0, '2017-01-01 00:00:00', 1), \
('News-Talk-1540', '', 'http://onair11.xdevel.com:8032', 'http://www.kxel.com/', 'News-Talk-1540.png', 0, 0, 0, 1, '2011-11-11', 2797, 0, '2017-01-01 00:00:00', 1), \
('Cat-Country-961', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', 'http://www.ksly.com/', 'Cat-Country-961.png', 0, 0, 0, 1, '2011-11-11', 3367, 0, '2017-01-01 00:00:00', 1), \
('The-Coast-973', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', 'http://coastfm.com/', 'The-Coast-973.png', 0, 0, 0, 1, '2011-11-11', 3368, 0, '2017-01-01 00:00:00', 1), \
('Michael-Jackson', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', '', 'Michael-Jackson.png', 0, 0, 0, 1, '2011-11-11', 3369, 0, '2017-01-01 00:00:00', 1), \
('The-Early-Hours', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', '', 'The-Early-Hours.png', 0, 0, 0, 1, '2011-11-11', 3370, 0, '2017-01-01 00:00:00', 1), \
('Frecuencia-Primera', '', 'http://67.212.179.138:8084', 'http://www.frecuenciaprimera.com/', 'Frecuencia-Primera.png', 0, 0, 0, 1, '2011-11-11', 3390, 0, '2017-01-01 00:00:00', 1), \
('Radio-Rewind', '', 'http://listen.radionomy.com/radio-rewind', 'http://radio-rewind.playtheradio.com/', 'Radio-Rewind.png', 0, 0, 0, 1, '2011-11-11', 3540, 0, '2017-01-01 00:00:00', 1), \
('WZSK-1040', '', 'http://listen.radionomy.com/radio-rewind', '', 'WZSK-1040.png', 0, 0, 0, 1, '2011-11-11', 3541, 0, '2017-01-01 00:00:00', 1), \
('Kronehit-Radio-Krone-Hit-1034', '', 'http://onair.krone.at:80/kronehit.mp3', 'http://www.kronehit.at/', 'Kronehit-Radio-Krone-Hit-1034.png', 0, 0, 0, 1, '2011-11-11', 3542, 0, '2017-01-01 00:00:00', 1), \
('181FM-Party-181', '', 'http://listen.181fm.com/181-party_128k.mp3', 'http://www.181.fm/', '181FM-Party-181.png', 0, 0, 0, 1, '2011-11-11', 3559, 0, '2017-01-01 00:00:00', 1), \
('3Wi-959', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=3wiradio', 'http://www.3wiradio.com/', '3Wi-959.png', 0, 0, 0, 1, '2011-11-11', 3565, 0, '2017-01-01 00:00:00', 1), \
('El-Gouna-Radio', '', 'http://82.201.132.237:8000', 'http://www.romolo.com/', 'El-Gouna-Radio-1000.png', 0, 0, 0, 1, '2011-11-11', 3574, 0, '2017-01-01 00:00:00', 1), \
('James-Morrison', '', 'http://82.201.132.237:8000', '', 'James-Morrison.png', 0, 0, 0, 1, '2011-11-11', 3575, 0, '2017-01-01 00:00:00', 1), \
('Full-Throttle', '', 'http://82.201.132.237:8000', '', 'Full-Throttle.png', 0, 0, 0, 1, '2011-11-11', 3576, 0, '2017-01-01 00:00:00', 1), \
('WERS-889', '', 'http://www.wers.org/wers.pls', 'http://www.wers.org/', 'WERS-889.png', 0, 0, 0, 1, '2011-11-11', 3577, 0, '2017-01-01 00:00:00', 1), \
('Jesus-Jones', '', 'http://www.wers.org/wers.pls', '', 'Jesus-Jones.png', 0, 0, 0, 1, '2011-11-11', 3579, 0, '2017-01-01 00:00:00', 1), \
('Crocodiles', '', 'http://www.wers.org/wers.pls', '', 'Crocodiles.png', 0, 0, 0, 1, '2011-11-11', 3581, 0, '2017-01-01 00:00:00', 1), \
('181FM-Highway-181', '', 'http://listen.181fm.com/181-highway_128k.mp3', 'http://www.181.fm/', '181FM-Highway-181.png', 0, 0, 0, 1, '2011-11-11', 3583, 0, '2017-01-01 00:00:00', 1), \
('House-Bomb-FN', '', 'http://stream.laut.fm/housebomb-fn', 'http://www.laut.fm/housebomb-fn', 'House-Bomb-FN.png', 16, 0, 0, 1, '2011-11-11', 3597, 0, '2017-01-01 00:00:00', 1), \
('Paul-Desmond', '', 'http://listen.radionomy.com/french-station', '', 'Paul-Desmond.png', 0, 0, 0, 1, '2011-11-11', 3627, 0, '2017-01-01 00:00:00', 1), \
('The-Wolf-933', '', 'http://listen.radionomy.com/french-station', 'http://www.933ncd.com/', 'The-Wolf-933.png', 0, 0, 0, 1, '2011-11-11', 3628, 0, '2017-01-01 00:00:00', 1), \
('Heart-Sussex-1035', 'Heart 102.4 &amp; 103.5 with Tom.', 'http://media-ice.musicradio.com/HeartSussexMP3', 'http://www.heartsussex.co.uk/', 'Heart-Sussex-1035.png', 0, 0, 0, 1, '2011-11-11', 3641, 0, '2017-01-01 00:00:00', 1), \
('181FM-90s', '', 'http://listen.181fm.com/181-90salt_128k.mp3', 'http://www.181.fm/', '181FM-90s-Alternative.png', 0, 0, 0, 1, '2011-11-11', 3647, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql8="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('World-FM-Tawa-882', 'World FM is a low power FM (LPFM) radio station based in Tawa, Wellington, New Zealand.<BR>Our aim is to play a mix of some of the best world music, Kiwi classics, and a selection of radio programming', 'http://www.worldfm.co.nz:8882/worldfmstereo.mp3', 'http://www.worldfm.co.nz/', 'World-FM-Tawa-882.png', 0, 0, 0, 1, '2011-11-11', 3756, 0, '2017-01-01 00:00:00', 1), \
('2b-FM', '2bfm was the first heard on February 12, 2007. The station focuses on Flanders. 2bfm is owned by Frederick Geeroms.<BR><BR>CHR-pop format with a focus 2bfm itself to a &#39;young adult&#39;audience. T', 'http://listen.radionomy.com/2bfm', 'http://www.2bfm.be/', '2b-FM.png', 0, 0, 0, 1, '2011-11-11', 3800, 0, '2017-01-01 00:00:00', 1), \
('Country-Music-24', '', 'http://countrymusic24.powerstream.de:9000', 'http://www.countrymusic24.com/', 'Country-Music-24.png', 6, 0, 0, 1, '2011-11-11', 3832, 0, '2017-01-01 00:00:00', 1), \
('KLSU-911', '', 'http://130.39.238.143:8002/listen.pls', 'http://www.klsu.fm/', 'KLSU-911.png', 0, 0, 0, 1, '2011-11-11', 3926, 0, '2017-01-01 00:00:00', 1), \
('2bfm-Classix', '', 'http://listen.radionomy.com/2bfm-classix', 'http://www.2bfm.be/', '2bfm-Classix.png', 0, 0, 0, 1, '2011-11-11', 3945, 0, '2017-01-01 00:00:00', 1), \
('Paul-OGrady', '', 'http://api.somafm.com/indiepop130.pls', '', 'Paul-OGrady.png', 0, 0, 0, 1, '2011-11-11', 3960, 0, '2017-01-01 00:00:00', 1), \
('Chris-Connor', '', 'http://stm1.rthk.hk/radio5', '', 'Chris-Connor.png', 0, 0, 0, 1, '2011-11-11', 3985, 0, '2017-01-01 00:00:00', 1), \
('ESPN-1013', 'ESPN affiliate station serving the Burlington, VT and Plattsburgh areas.  Flagship station for the University of Vermont Men&#39;s basketball team and the Vermont Lake Monsters minor league baseball t', 'http://crystalout.surfernetwork.com:8001/WCPV_MP3', 'http://champrocks.com/', 'ESPN-1013.png', 0, 0, 0, 1, '2011-11-11', 3990, 0, '2017-01-01 00:00:00', 1), \
('No-Secrets', '', 'http://crystalout.surfernetwork.com:8001/WCPV_MP3', '', 'No-Secrets.png', 0, 0, 0, 1, '2011-11-11', 3992, 0, '2017-01-01 00:00:00', 1), \
('WKHG-1049', '', 'http://crystalout.surfernetwork.com:8001/WKHG_MP3', 'http://www.k105.com/', 'WKHG-1049.png', 0, 0, 0, 1, '2011-11-11', 4040, 0, '2017-01-01 00:00:00', 1), \
('Vox-Angeli', '', 'http://crystalout.surfernetwork.com:8001/WKHG_MP3', '', 'Vox-Angeli.png', 0, 0, 0, 1, '2011-11-11', 4041, 0, '2017-01-01 00:00:00', 1), \
('PMD-Radio', 'We play the best 80&#39;s and 90&#39;s hits The best movie tunes and the best Disney music from the movies and the parks around the world.<BR><BR>Want your favorite song on PMD-Radio than send us an e', 'http://listen.radionomy.com/pmd-radio', 'http://www.radionomy.com/pmd-radio', 'PMD-Radio.png', 0, 0, 0, 1, '2011-11-11', 4074, 0, '2017-01-01 00:00:00', 1), \
('NAXI RADIO 96', '9MHz - Beograd - www.naxi.rs ', 'http://193.243.169.34:9150', '', '', 0, 128, 1, 1, '2011-08-11', 4247, 45, '2011-10-14 16:11:02', 1), \
('WKSU Classical', 'WKSU Classical: Ludwig van Beethoven - Piano Sonata No. 31  [Bitrate: 128] [Max Listeners: 2000]', 'http://66.225.205.8:8030', '', '', 8, 128, 0, 1, '2011-08-11', 4275, 7, NULL, 1), \
('NOVA-fm', 'Danmarks st&#248;rste kommercielle radiostation.', 'http://stream.novafm.dk/nova128', 'http://novafm.dk/', 'NOVA-fm-914.png', 0, 0, 0, 1, '2011-11-11', 4827, 0, '2017-01-01 00:00:00', 1), \
('Progressive-Radio-Network', 'Progressive Radio Network is the thinking person', 'http://216.55.165.146:8000', 'http://www.progressiveradionetwork.com/', 'Progressive-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 4868, 0, '2017-01-01 00:00:00', 1), \
('Kass-FM-891', '', 'http://media.kassfm.co.ke:8006/live', 'http://www.kassfm.co.ke/', 'Kass-FM-891.png', 0, 0, 0, 1, '2011-11-11', 4894, 0, '2017-01-01 00:00:00', 1), \
('RTL-2-1062', '', 'http://radio-shoutcast.cyber-streaming.com:8090/', 'http://www.rtl2antilles.fr/', 'RTL-2-Guadeloupe-1062.png', 0, 0, 0, 1, '2011-11-11', 4920, 0, '2017-01-01 00:00:00', 1), \
('Fun-Radio-943', '', 'http://stream.funradio.sk:8000/fun128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-943.png', 0, 0, 0, 1, '2011-11-11', 4942, 0, '2017-01-01 00:00:00', 1), \
('Kass-FM-891', '', 'http://media.kassfm.co.ke:8006/live', 'http://www.kassfm.co.ke/', 'Kass-FM-891.png', 0, 0, 0, 1, '2011-11-11', 5019, 0, '2017-01-01 00:00:00', 1), \
('Ambiance-Reggae', 'Une webradio &#224; la programmation musicale 100% reggae, Dub, Pop-Reggae, Ragga et Roots Reggae, en continu et en direct de Paris.', 'http://listen.radionomy.com/ambiance-reggae', 'http://ambiance-reggae.playtheradio.com/', 'Ambiance-Reggae.png', 0, 0, 1, 1, '2011-11-11', 5026, 0, '2011-10-14 16:00:11', 1), \
('Fun-Radio-943', '', 'http://stream.funradio.sk:8000/fun128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-943.png', 0, 0, 0, 1, '2011-11-11', 5067, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql13="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Cheche-80S', 'Esta es una emisora especializada en clasicos, especialmente de las decadas 80&#180;s y 90&#180;s. La programacion abarca los diversos subgeneros del rock. Escucharas las bandas, grupos y artistas con', 'http://listen.radionomy.com/cheche-international-radio', 'http://www.elcheche.net', 'Cheche-80S--90S-Rock-Hits.png', 0, 0, 0, 1, '2011-11-11', 5324, 0, '2017-01-01 00:00:00', 1), \
('Diamond-Gems', '', 'http://stream.laut.fm/jazzloft', '', 'Diamond-Gems.png', 0, 0, 0, 1, '2011-11-11', 5359, 0, '2017-01-01 00:00:00', 1), \
('KBLU-560', '', 'http://asx.abacast.com/eldorado-kblu-64.pls', 'http://www.kbluam.com/', 'KBLU-560.png', 0, 0, 0, 1, '2011-11-11', 5360, 0, '2017-01-01 00:00:00', 1), \
('AirProgressiveorg', 'Progressive News and Talk radio from around the nation, with shows updated hourly. Live shows coming soon.', 'http://www.airprogressive.org:8000/stream', 'http://www.airprogressive.org/', 'AirProgressiveorg.png', 0, 0, 0, 1, '2011-11-11', 5369, 0, '2017-01-01 00:00:00', 1), \
('SAW-80er', '', 'http://stream.radiosaw.de/saw-80er/mp3-128/tunein/', 'http://www.radiosaw.de/', 'SAW-80er.png', 0, 0, 0, 1, '2011-11-11', 5372, 0, '2017-01-01 00:00:00', 1), \
('Mix-967', '', 'http://stream.radiosaw.de/saw-80er/mp3-128/tunein/', 'http://www.mix96.fm/', 'Mix-967.png', 0, 0, 0, 1, '2011-11-11', 5373, 0, '2017-01-01 00:00:00', 1), \
('City-Dance-Radio', 'City Pop Radio&#169; la emisora online en Valencia (Espa&#241;a) que te har&#225; disfrutar de todos los &#233;xitos del pop latino e internacional desde los 80 hasta la actualidad, con una variad&#23', 'http://listen.radionomy.com/city-dance-radio', 'http://www.citypopradio.es/', 'City-Dance-Radio.png', 0, 0, 0, 1, '2011-11-11', 5414, 0, '2017-01-01 00:00:00', 1), \
('Norea-DK', '', 'http://radio.norea.dk:8080', 'http://www.norea.dk/', 'Norea-DK.png', 0, 0, 0, 1, '2011-11-11', 5483, 0, '2017-01-01 00:00:00', 1), \
('Laid-Back-Radio', 'Laid Back Radio (LDBK) plays everything from Soul, Funk, Jazz, Psych Rock, Disco and Latin music to Hip Hop and much more.', 'http://listen.radionomy.com/Laid-Back-Radio', 'http://www.laid-back.be/', 'Laid-Back-Radio.png', 0, 0, 0, 1, '2011-11-11', 5591, 0, '2017-01-01 00:00:00', 1), \
('Skaga-FM-887', '', 'http://media.wlmm.dk:80/skagafm', 'http://www.skagafm.dk/', 'Skaga-FM-887.png', 0, 0, 0, 1, '2011-11-11', 5639, 0, '2017-01-01 00:00:00', 1), \
('Heart-London-1062', 'Heart 106.2 with Jamie &amp; Harriet at Breakfast covers Greater London', 'http://media-ice.musicradio.com/HeartLondonMP3', 'http://www.heartlondon.co.uk/', 'Heart-London-1062.png', 0, 0, 0, 1, '2011-11-11', 5641, 0, '2017-01-01 00:00:00', 1), \
('Mixnation-1045', 'MixNation ist einzigartig und spielt Re-Mixe aus dem Bereich Pop, Rock, R&#39;n&#39;B und Hip-Hop.', 'http://sc1.netstreamer.net:8090', 'http://www.mixnation.de/', 'Mixnation-Radio-1045.png', 0, 0, 0, 1, '2011-11-11', 6282, 0, '2017-01-01 00:00:00', 1), \
('Power-1035', '', 'http://crystalout.surfernetwork.com:8001/KVSP_MP3', 'http://www.kvsp.com/', 'Power-1035.png', 0, 0, 0, 1, '2011-11-11', 6307, 0, '2017-01-01 00:00:00', 1), \
('Radio-88---Retro-88-954', '', 'http://stream1.radio88.hu:8300/', 'http://www.radio88.hu/', 'Radio-88---Retro-88-954.png', 0, 0, 0, 1, '2011-11-11', 6332, 0, '2017-01-01 00:00:00', 1), \
('Radio-Truckerladen', '', 'http://stream.laut.fm/truckerladen', 'http://www.truckerladen.de/', 'Radio-Truckerladen.png', 0, 0, 0, 1, '2011-11-11', 6456, 0, '2017-01-01 00:00:00', 1), \
('Rachel-Platten', '', 'http://stream.laut.fm/truckerladen', '', 'Rachel-Platten.png', 0, 0, 0, 1, '2011-11-11', 6458, 0, '2017-01-01 00:00:00', 1), \
('Den-Gale-Pose', '', 'http://stream.laut.fm/truckerladen', '', 'Den-Gale-Pose.png', 0, 0, 0, 1, '2011-11-11', 6459, 0, '2017-01-01 00:00:00', 1), \
('Lasgo', '', 'http://stream.laut.fm/truckerladen', '', 'Lasgo.png', 0, 0, 0, 1, '2011-11-11', 6460, 0, '2017-01-01 00:00:00', 1), \
('KINO-1230', '', 'http://stream.laut.fm/truckerladen', '', 'KINO-1230.png', 0, 0, 0, 1, '2011-11-11', 6461, 0, '2017-01-01 00:00:00', 1), \
('Radio-Torino', 'Radio Torino International - La Radio di Torino per la comunit&#224; Romena.', 'http://stream15.top-ix.org:80/radiotorinointernational', 'http://www.torinointernational.com/', 'Radio-Torino-International-900.png', 0, 0, 0, 1, '2011-11-11', 6464, 0, '2017-01-01 00:00:00', 1), \
('RadioSky-Music-Jazz', 'WebRadio essentially musical, styles in: Jazz, Vocal Jazz, Blues, Swing, BigBand, Latin Jazz, Caribbean, Brazil...', 'http://listen.radionomy.com/radiosky-music', 'http://radiosky-music.playtheradio.com', 'RadioSky-Music-Jazz.png', 0, 0, 0, 1, '2011-11-11', 6611, 0, '2017-01-01 00:00:00', 1), \
('Felger-Massarotti', '', 'http://media-ice.musicradio.com/HeartOxfordshireMP3', '', 'Felger-and-Massarotti.png', 0, 0, 0, 1, '2011-11-11', 6735, 0, '2017-01-01 00:00:00', 1), \
('90s-Party', 'Hits des annes 90', 'http://listen.radionomy.com/90-s-party', 'http://www.90-s-party.com/', '90s-Party.png', 0, 0, 1, 1, '2011-11-11', 6761, 0, '2011-10-14 15:57:11', 1);";


const char *radio_station_setupsql17="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR P4 Nordjylland', '', 'http://live-icy.gss.dr.dk:8000/A/A10H.mp3', 'http://www.dr.dk/p4/nord/', 'DR-P4-Nordjylland-981.png', 0, 0, 0, 1, '2011-11-11', 7000, 0, '2017-01-01 00:00:00', 1), \
('DR Klassisk', '', 'http://live-icy.gss.dr.dk:8000/A/A04H.mp3', 'http://www.dr.dk/radio/', 'DR-Klassisk.png', 0, 0, 0, 1, '2011-11-11', 7331, 0, '2017-01-01 00:00:00', 1), \
('Dragonland-Radio-960', 'Das Dragonland-Radio sorgt mit der Partynight, dem Kaffeeklatsch, dem Spa&#223; am Mittag und anderen Sendungen f&#252;r Ihre musikalische Unterhaltung.', 'http://www.soulfulbits.com/files/streaming/soulfulbits-radio.m3u', 'http://www.dragonland-radio.de/', 'Dragonland-Radio-960.png', 0, 0, 0, 1, '2011-11-11', 7777, 0, '2017-01-01 00:00:00', 0), \
('Patriotscom-Radio', 'Official online radio station of the New England Patriots, featuring shows such as PFW in Progress, The Pete Sheppard Show, Patriots Playbook, Patriots Monday, Patriots Friday, Fantasy Lowe Down, and ', 'http://www.soulfulbits.com/files/streaming/soulfulbits-radio.m3u', 'http://www.patriots.com/', 'Patriotscom-Radio.png', 0, 0, 0, 1, '2011-11-11', 7780, 0, '2017-01-01 00:00:00', 0), \
('Mietta', '', 'http://www.soulfulbits.com/files/streaming/soulfulbits-radio.m3u', '', 'Mietta.png', 0, 0, 0, 1, '2011-11-11', 7785, 0, '2017-01-01 00:00:00', 0), \
('Sandra', '', 'http://listen.radionomy.com/M-B-C-Radio', '', 'Sandra.png', 0, 0, 0, 1, '2011-11-11', 7917, 0, '2017-01-01 00:00:00', 1), \
('Nummer-1-Oldies', 'Klasse Musik zum Entspannen oder Mitsingen', 'http://stream.laut.fm/nummer_1_oldies', 'http://www.laut.fm/nummer_1_oldies', 'Nummer-1-Oldies.png', 0, 0, 0, 1, '2011-11-11', 7962, 0, '2017-01-01 00:00:00', 1), \
('Old-Love-Songs', '', 'http://listen.181fm.com/181-heart_128k.mp3', 'http://www.181.fm/', '', 0, 0, 0, 1, '2011-11-11', 8003, 0, '2017-01-01 00:00:00', 1), \
('Radio-Skive', '', 'http://netradio.radioskive.dk', 'http://www.radioskive.dk/', 'Radio-Skive-1043.png', 0, 0, 0, 1, '2011-11-11', 8034, 0, '2017-01-01 00:00:00', 1), \
('Celtica-Radio', 'From South Wales', 'http://51.255.235.165:5232/stream', 'http://www.celticaradio.com/', 'Celtica-Radio.png', 0, 0, 0, 1, '2011-11-11', 8114, 0, '2017-01-01 00:00:00', 1), \
('Radio-K-770', 'Radio K is the award-winning student-run radio station of the University of Minnesota, playing an eclectic variety of independent music both old and new.  <BR><BR>Radio K educates students, breaks gro', 'http://radiokstreams.cce.umn.edu:8128', 'http://radiok.cce.umn.edu/', 'Radio-K-770.png', 0, 0, 0, 1, '2011-11-11', 8119, 0, '2017-01-01 00:00:00', 1), \
('Ryan-Adams', '', 'http://streaming.radionomy.com/100-HIT-radio', '', 'Ryan-Adams.png', 0, 0, 0, 1, '2011-11-11', 8124, 0, '2017-01-01 00:00:00', 1), \
('The-Mix', '', 'http://streaming08.mit.ovea.com/medi1', '', 'The-Mix.png', 0, 0, 0, 1, '2011-11-11', 8183, 0, '2017-01-01 00:00:00', 1), \
('Heart-Bristol-963', 'Heart 96.3 with Bush, Troy and Paulina at breakfast covers Bristol.', 'http://media-ice.musicradio.com/HeartBristolMP3', 'http://www.heart.co.uk/bristol/', 'Heart-Bristol-963.png', 0, 0, 0, 1, '2011-11-11', 8253, 0, '2017-01-01 00:00:00', 1), \
('Goa-Trance-Chillout', '', 'http://komplex2.psyradio.org:8020/', 'http://www.goa-trance.de/', 'Goa-Trance-Chillout.png', 0, 0, 0, 1, '2011-11-11', 8331, 0, '2017-01-01 00:00:00', 1), \
('Ocean-Beach-Radio', 'It&#39;s only a little bit of Rock &#39;N Roll, and a lot of Americana and Country, Folk, Blues, Jazz and some other stuff......', 'http://ophanim.net:7130/', 'http://www.oceanbeachradio.com/', 'Ocean-Beach-Radio.png', 0, 0, 0, 1, '2011-11-11', 8412, 0, '2017-01-01 00:00:00', 1), \
('Mega-Rock-1055', '', 'http://ophanim.net:7130/', 'http://www.megarock.fm/', 'Mega-Rock-1055.png', 0, 0, 0, 1, '2011-11-11', 8413, 0, '2017-01-01 00:00:00', 1), \
('181FM-The-Mix', '', 'http://listen.181fm.com/181-themix_128k.mp3', 'http://www.181.fm/', '181FM-The-Mix.png', 0, 0, 0, 1, '2011-11-11', 8435, 0, '2017-01-01 00:00:00', 1), \
('Mayhem-in-the-AM', '', 'http://listen.radionomy.com/Play-Misty-for-Me', '', 'Mayhem-in-the-AM.png', 0, 0, 0, 1, '2011-11-11', 8440, 0, '2017-01-01 00:00:00', 1), \
('KPOO-895', 'KPOO is an independent, listener-sponsored noncommercial station. KPOO is an African-American owned and operated noncommercial radio station. KPOO broadcasts 24 hours a day on 160 watts, with a radiat', 'http://amber.streamguys.com:5220/xstream', 'http://www.kpoo.com/', 'KPOO-895.png', 0, 0, 0, 1, '2011-11-11', 8481, 0, '2017-01-01 00:00:00', 1), \
('KKOL-1300', '', 'http://media-ice.musicradio.com/HeartEssexMP3', 'http://kol.townhall.com/', 'KKOL-1300.png', 0, 0, 0, 1, '2011-11-11', 8550, 0, '2017-01-01 00:00:00', 1), \
('WSIE-887', 'Serving the Greater St. Louis Area and Southwestern Illinois', 'http://streaming.siue.edu:8000/wsie.mp3', 'http://www.wsie.com', 'WSIE-887.png', 0, 0, 0, 1, '2011-11-11', 8593, 0, '2017-01-01 00:00:00', 1), \
('The-Voice-1049', 'Danmarks st&#248;rste ungdomsbrand', 'http://stream.voice.dk/voice128', 'http://www.thevoice.dk/', 'The-Voice-1049.png', 0, 0, 0, 1, '2011-11-11', 8615, 0, '2017-01-01 00:00:00', 1), \
('KAT-103-1037', '', 'http://stream.voice.dk/voice128', 'http://www.thekat.com/', 'KAT-103-1037.png', 0, 0, 0, 1, '2011-11-11', 8617, 0, '2017-01-01 00:00:00', 1), \
('WAYV-951', '', 'http://crystalout.surfernetwork.com:8001/WAYV_MP3', 'http://www.951wayv.com/', 'WAYV-951.png', 0, 0, 0, 1, '2011-11-11', 8674, 0, '2017-01-01 00:00:00', 1), \
('80-EXITS', '', 'http://listen.radionomy.com/80-exits', 'http://www.hospitaletfm.com/80exits.php', '80-EXITS.png', 0, 0, 0, 1, '2011-11-11', 8798, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Midt Vest', '', 'http://live-icy.gss.dr.dk:8000/A/A09H.mp3', 'http://www.dr.dk/p4/vest/', 'DR-P4-Midt--Vest-985.png', 0, 0, 0, 1, '2011-11-11', 8807, 0, '2017-01-01 00:00:00', 1), \
('The-Planet-967', '', 'http://crystalout.surfernetwork.com:8001/WXZO_MP3', 'http://www.theplanet967.com/', 'The-Planet-967.png', 0, 0, 0, 1, '2011-11-11', 8912, 0, '2017-01-01 00:00:00', 1), \
('NRK-Hordaland', '', 'http://lyd.nrk.no/nrk_radio_p1_hordaland_mp3_h', 'http://www.nrk.no/hordaland/', 'NRK-P1-Hordaland-891.png', 0, 0, 0, 1, '2011-11-11', 8947, 0, '2017-01-01 00:00:00', 1), \
('Lester-Young', '', 'http://media-ice.musicradio.com/HeartKentMP3', '', 'Lester-Young.png', 0, 0, 0, 1, '2011-11-11', 9028, 0, '2017-01-01 00:00:00', 1), \
('Ida-Corr', '', 'http://studio.phoenixhospitalradio.com:8000/stream.mp3', '', 'Ida-Corr.png', 0, 0, 0, 1, '2011-11-11', 9096, 0, '2017-01-01 00:00:00', 1), \
('WELI-960', '', 'http://studio.phoenixhospitalradio.com:8000/stream.mp3', 'http://www.weliam.com/', 'WELI-960.png', 0, 0, 0, 1, '2011-11-11', 9097, 0, '2017-01-01 00:00:00', 1), \
('Puls-FM-1033', '', 'http://firewall.pulsradio.com', 'http://www.pulsfm.ru/', 'Puls-FM-1033.png', 0, 0, 0, 1, '2011-11-11', 9098, 0, '2017-01-01 00:00:00', 1), \
('Active-Gaydance', 'Night club music all the day 24/7 on RainbowWebradio.com', 'http://listen.radionomy.com/Rainbow-Webradio', 'http://www.rainbowwebradio.com', 'Active-Gaydance.png', 0, 0, 0, 1, '2011-11-11', 9172, 0, '2017-01-01 00:00:00', 1), \
('Antwerpen-fm-1054', '', 'http://streams.lazernet.be:2710/', 'http://www.antwerpen.fm', 'Antwerpen-fm-1054.png', 0, 0, 0, 1, '2011-11-11', 9180, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Sj\xE6lland', '', 'http://live-icy.gss.dr.dk:8000/A/A11H.mp3', 'http://www.dr.dk/p4/sjaelland/', 'DR-P4-Sjaelland-975.png', 0, 0, 0, 1, '2011-11-11', 9226, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql27="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Radio-Caroline-319-Gold', '', 'http://www.rcgoldserver.eu:8192', 'http://www.radiocaroline319.eu/', 'Radio-Caroline-319-Gold.png', 0, 0, 0, 1, '2011-11-11', 9336, 0, '2017-01-01 00:00:00', 1), \
('Fun-Radio-Dance', '24-hour stream of dance music, from the latest hits to the 90-ies, designed especially for diskomaniakov.', 'http://stream.funradio.sk:8000/dance128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-Dance.png', 0, 0, 0, 1, '2011-11-11', 9449, 0, '2017-01-01 00:00:00', 1), \
('Club--Dance-Radio', '', 'http://listen.radionomy.com/club-dance', 'http://www.clubanddance.com/', 'Club--Dance-Radio.png', 0, 0, 0, 1, '2011-11-11', 9453, 0, '2017-01-01 00:00:00', 1), \
('Retro--Rock--Pop', 'El Grupo Radial M&amp;M y su Emisora Online Retro &#169; Rock &amp; Pop, transmite su se&#241;al desde Lima - Per&#250; para todo el mundo las 24 horas del d&#237;a y durante los 7 d&#237;as de la sem', 'http://listen.radionomy.com/radio-retro-rock--pop', 'http://www.radioretro.comlu.com/', 'Retro--Rock--Pop.png', 0, 0, 0, 1, '2011-11-11', 9595, 0, '2017-01-01 00:00:00', 1), \
('JPHiP-Radio', 'The latest in Jpop, Kpop, Cpop, HiP HoP.', 'http://radio.jphip.com:8800', 'http://jphip.com/radio.html', 'JPHiP-Radio.png', 0, 0, 0, 1, '2011-11-11', 9619, 0, '2017-01-01 00:00:00', 1), \
('Classic-Rap', 'Classic Rap est une Web radio, France.', 'http://listen.radionomy.com/classic-rap', 'http://classicrap.fr/', 'Classic-Rap.png', 0, 0, 0, 1, '2011-11-11', 9622, 0, '2017-01-01 00:00:00', 1), \
('Allegro---Jazz', '', 'http://listen.radionomy.com/allegro-jazz', 'http://www.allegro-radio.com/jazz.html', 'Allegro---Jazz.png', 0, 0, 1, 1, '2011-11-11', 9779, 0, '2011-10-15 20:12:11', 1), \
('FM-Classic-Hits-181', 'Home of The 60&#39;s and 70&#39;s', 'http://listen.181fm.com/181-greatoldies_128k.mp3', 'http://www.181.fm/', '181FM-Classic-Hits-181.png', 0, 0, 0, 1, '2011-11-11', 9800, 0, '2017-01-01 00:00:00', 1), \
('Black-Eyed-Peas', '', 'http://listen.radionomy.com/abcd-black-eyed-peas', 'http://www.abcdeurodance.com', 'ABCD-Black-Eyed-Peas.png', 0, 0, 1, 1, '2011-11-11', 9842, 0, '2011-10-14 15:59:07', 1), \
('SAW-70er', '', 'http://stream.radiosaw.de/saw-70er/mp3-128/tunein/', 'http://www.radiosaw.de/', 'SAW-70er.png', 0, 0, 0, 1, '2011-11-11', 10036, 0, '2017-01-01 00:00:00', 1), \
('Juli', '', 'http://listen.radionomy.com/abc-lounge', '', 'Juli.png', 0, 0, 0, 1, '2011-11-11', 10125, 0, '2017-01-01 00:00:00', 1), \
('Kiss-FM-1059', '', 'http://webcast.wonc.org:8000/wonclive-128s', 'http://www.kissdetroit.com/', 'Kiss-FM-1059.png', 0, 0, 0, 1, '2011-11-11', 10238, 0, '2017-01-01 00:00:00', 1), \
('WSOU-895', '', 'http://crystalout.surfernetwork.com:8001/WSOU_MP3', 'http://www.wsou.net/', 'WSOU-895.png', 0, 0, 0, 1, '2011-11-11', 10239, 0, '2017-01-01 00:00:00', 1), \
('Radio-Generation-Do', '', 'http://listen.radionomy.com/generationdo', '', 'Radio-Generation-Do.png', 0, 0, 0, 1, '2011-11-11', 10246, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql31="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR Nyheder', '', 'http://live-icy.gss.dr.dk:8000/A/A02H.mp3', 'http://www.dr.dk/radio/alle_kanaler/nyheder.asp', 'DR-Nyheder.png', 0, 0, 0, 1, '2011-11-11', 11396, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Trekanten', '', 'http://live-icy.gss.dr.dk:8000/A/A13H.mp3', 'http://www.dr.dk/p4/trekanten/', 'DR-P4-Trekanten-940.png', 0, 0, 0, 1, '2011-11-11', 11450, 0, '2017-01-01 00:00:00', 1), \
('DR P5', '', 'http://live-icy.gss.dr.dk:8000/A/A25H.mp3.m3u', 'http://www.bgradio.bg/', '', 0, 0, 0, 1, '2017-01-01', 14362, 0, '2017-01-01 00:00:00', 1), \
('Radio Soft', '', 'http://onair.100fmlive.dk/soft_live.mp3.m3u', 'http://www.radioplay.dk/radiosoft', '', 0, 0, 0, 1, '2017-01-01', 14364, 0, '2017-01-01 00:00:00', 1), \
('Tiger FM', '', 'http://85.202.67.163:8000/stream.mp3.m3u', 'http://www.tigerfm.dk/', '', 0, 0, 0, 1, '2017-01-01', 14365, 0, '2017-01-01 00:00:00', 1), \
('PARTY VIBE', 'TECHNO-HOUSE-TRANCE','http://107.182.233.214:8046/stream', 'https://www.partyvibe.com', '', 0, 0, 0, 1, '2017-01-01', 14367, 0, '2017-01-01 00:00:00', 1), \
('Chillkyway', 'TECHNO','http://136.243.16.19:8000/stream/3/', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14368 , 0, '2017-01-01 00:00:00', 1), \
('Audiophile Jazz', 'JAZZ','http://8.38.78.173:8210/stream', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14369 , 0, '2017-01-01 00:00:00', 1), \
('Cosmic Radio Chillout', 'Trance','http://38.109.219.207:8020/', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14370 , 0, '2017-01-01 00:00:00', 1), \
('Trancemission', 'Chillout','http://uk2.internet-radio.com:31491/', 'http://www.chillkyway.net', '', 4, 0, 0, 1, '2017-01-01',14370 , 0, '2017-01-01 00:00:00', 1)";

const char *radio_station_setupsql39="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR P1', '', 'http://live-icy.gss.dr.dk:8000/A/A03H.mp3', 'http://www.dr.dk/p1/', 'DR-P1-908.png', 0, 0, 0, 1, '2011-11-11', 12822, 0, '2017-01-01 00:00:00', 1);";

const char *radio_station_setupsql40="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Spirit-1059', 'SPIRIT 105.9, The Spirit of Austin, features Adult Contemporary Christian music; a popular music format that appeals to a wide range of listeners.', 'http://crista-kfmk.streamguys1.com/kfmkaacp.m3u', 'http://www.spirit1059.com/', 'Spirit-1059.png', 0, 0, 0, 1, '2011-11-11', 13176, 0, '2017-01-01 00:00:00', 1), \
('Dance90s', '', 'http://listen.radionomy.com/dance90', 'http://dance90.tk/', 'Dance90s.png', 0, 0, 0, 1, '2011-11-11', 13185, 0, '2017-01-01 00:00:00', 1), \
('WESC-FM-925', '', 'http://listen.radionomy.com/dance90', 'http://www.wescfm.com/', 'WESC-FM-925.png', 0, 0, 0, 1, '2011-11-11', 13186, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql41="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Lite-Rock-973', '', 'http://listen.radionomy.com/eurodance-90', 'http://www.literock973.com/', 'Lite-Rock-973.png', 0, 0, 0, 1, '2011-11-11', 13277, 0, '2017-01-01 00:00:00', 1), \
('Streekradio-1048', '', 'http://livestream.streekradio.com/live', 'http://www.streekradio.com/', 'Streekradio-1048.png', 0, 0, 0, 1, '2011-11-11', 13375, 0, '2017-01-01 00:00:00', 1), \
('Radio-Metro-1024', '', 'http://195.182.132.18:8230', 'http://radiometro.ru/', 'Radio-Metro-1024.png', 0, 0, 0, 1, '2011-11-11', 13418, 0, '2017-01-01 00:00:00', 1), \
('DI-Lounge', 'Sit back and enjoy the lounge grooves!', 'http://listen.di.fm/partner_mp3/lounge.pls', 'http://www.di.fm/lounge', 'DI-Lounge.png', 0, 0, 0, 1, '2011-11-11', 13460, 0, '2017-01-01 00:00:00', 1);";

const char *radio_station_setupsql42="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Easy-Network-987', 'Easy Network , storica emittente di Veneto e Friuli Venezia Giulia, grazie alle sua diffusione capillare e alla sua programmazione musicale', 'http://str01.fluidstream.net:6030', 'http://www.easynetwork.fm/', 'Easy-Network-987.png', 0, 0, 0, 1, '2011-11-11', 13477, 0, '2017-01-01 00:00:00', 1), \
('RFD-Illinois', '', 'http://str01.fluidstream.net:6030', '', 'RFD-Illinois.png', 0, 0, 0, 1, '2011-11-11', 13479, 0, '2017-01-01 00:00:00', 1), \
('AFR-Talk-917', 'American Family Radio (AFR) is a network of more than 180 radio stations broadcasting Christian-oriented programming to over 40 states. AFR was launched by Rev. Donald Wildmon in 1991 as a ministry of', 'http://mediaserver3.afa.net:8000/talkhigh.mp3', 'http://www.afr.net/', 'AFR-Talk-917.png', 0, 0, 0, 1, '2011-11-11', 13586, 0, '2017-01-01 00:00:00', 1), \
('Dive-Bar', '', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.charlestonillustrated.com/radio.htm', 'Dive-Bar-Jukebox-Radio.png', 0, 0, 0, 1, '2011-11-11', 13598, 0, '2017-01-01 00:00:00', 1), \
('Jimmie-Rodgers', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Jimmie-Rodgers.png', 0, 0, 0, 1, '2011-11-11', 13599, 0, '2017-01-01 00:00:00', 1), \
('D-FM-902', '', 'http://striiming.trio.ee/dfm.mp3', 'http://www.dfm.ee/', 'D-FM-902.png', 0, 0, 0, 1, '2011-11-11', 13609, 0, '2017-01-01 00:00:00', 1), \
('Stevie-Storm', '', 'http://striiming.trio.ee/dfm.mp3', '', 'Stevie-Storm.png', 0, 0, 0, 1, '2011-11-11', 13610, 0, '2017-01-01 00:00:00', 1), \
('Radio-SLR-1010', '', 'http://netradio.radioslr.dk', 'http://www.radioslr.dk/', 'Radio-SLR-1010.png', 0, 0, 0, 1, '2011-11-11', 13617, 0, '2017-01-01 00:00:00', 1), \
('No-Holds-Barred', '', 'http://www.newerastreaming.com:8000', 'http://www.nhbradio.com/', 'No-Holds-Barred-Radio.png', 0, 0, 0, 1, '2011-11-11', 13627, 0, '2017-01-01 00:00:00', 1), \
('All-Dance-Radio', '', 'http://listen.radionomy.com/all-dance-radio', 'http://www.radionomy.com/en/radio/all-dance-radio', 'All-Dance-Radio.png', 0, 0, 1, 1, '2011-11-11', 13717, 0, '2011-10-15 20:11:45', 1), \
('Farm-Radio-1010', '', 'http://alpha.newerastreaming.com:8016', 'http://www.ksir.com/', 'Farm-Radio-1010.png', 0, 0, 0, 1, '2011-11-11', 13720, 0, '2017-01-01 00:00:00', 1), \
('Z-104-1041', '', 'http://alpha.newerastreaming.com:8016', 'http://www.z104fm.com/', 'Z-104-1041.png', 0, 0, 0, 1, '2011-11-11', 13721, 0, '2017-01-01 00:00:00', 1), \
('Fun101-FM-1011', 'Todays Hits, Five decades of hits and Classic Rock.', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wtga-fm', 'http://www.fun101fm.com', 'Fun101-FM-1011.png', 0, 0, 0, 1, '2011-11-11', 13735, 0, '2017-01-01 00:00:00', 1), \
('Juice-1072', 'Juice 107.2 is The Local Radio Station for Brighton &amp; Hove, UK. Broadcasting LIVE from North Street', 'http://tx.sharp-stream.com/icecast.php?i=juice1072.mp3', 'http://www.juicebrighton.com/', 'Juice-1072.png', 0, 0, 0, 1, '2011-11-11', 13737, 0, '2017-01-01 00:00:00', 1);";



const char *radio_station_setupsql43="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Foster-the-People', '', 'http://netradio.radioalfa.dk', '', 'Foster-the-People.png', 0, 0, 0, 1, '2011-11-11', 13750, 0, '2017-01-01 00:00:00', 1), \
('DR P3', '', 'http://live-icy.gss.dr.dk:8000/A/A05H.mp3', 'http://www.dr.dk/p3/', 'DR-P3-939.png', 0, 0, 0, 1, '2011-11-11', 13772, 0, '2017-01-01 00:00:00', 1), \
('HitKast', '', 'https://d1qg6pckcqcdk0.cloudfront.net/chr2/kushnerdavid_daylightsingle_01_daylight.m4a', '', 'https://cdn.accuradio.com/static/images/covers300/covers/g-m/kushnerdavid_daylightsingle.jpg', 0, 0, 0, 1, '2023-10-07', 13772, 0, '2023-10-07 00:00:00', 1), \
('1990s R&B', '', 'https://d1qg6pckcqcdk0.cloudfront.net/chr/maryjblige-youremindme-clean.m4a', '', 'https://cdn.accuradio.com/static/images/covers300/covers/a-f/bligemaryj_whatsthe411.jpg', 0, 0, 0, 1, '2023-10-07', 13772, 0, '2023-10-07 00:00:00', 1), \
('WEPN-FM', '', 'https://prod-3-93-164-218.amperwave.net/goodkarma-wepnfmmp3-ibc?', '', 'https://www.radio.net/images/broadcasts/07/b9/23405/c300.png', 0, 0, 0, 1, '2023-10-07', 13772, 0, '2023-10-07 00:00:00', 1), \
('101 Smooth jazz', '', 'https://prod-3-93-164-218.amperwave.net/goodkarma-wepnfmmp3-ibc?', '', 'c300.png', 0, 0, 0, 1, '2023-10-07', 13772, 0, '2023-10-07 00:00:00', 1), \
('Radio-100-1036', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://radio100.dk', 'Radio-100-1036.png', 0, 0, 0, 1, '2011-11-11', 13773, 0, '2017-01-01 00:00:00', 1), \
('181FM-Trance-Jazz', '', 'http://icyrelay.181.fm/181-trancejazz_128k.mp3', 'http://www.181.fm/', '181FM-Trance-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13802, 0, '2017-01-01 00:00:00', 1), \
('ZaycevFM-RnB', '', 'http://www.zaycev.fm:9002/rnb/ZaycevFM(256)', 'http://www.zaycev.fm', 'ZaycevFM-RnB.png', 0, 0, 0, 1, '2011-11-11', 13907, 0, '2017-01-01 00:00:00', 1), \
('WKXL-1450', '', 'http://wkxl.serverroom.us:7516', 'http://wkxl1450.com/', 'WKXL-1450.png', 0, 0, 0, 1, '2011-11-11', 13915, 0, '2017-01-01 00:00:00', 1), \
('Old-School-Radio', 'Les classiques du funk,dela soul et du hip hop en continu sur Old Schol radio !', 'http://listen.radionomy.com/oldschool', 'http://www.oldschool-radio.com/', 'Old-School-Radio.png', 0, 0, 0, 1, '2011-11-11', 13946, 0, '2017-01-01 00:00:00', 1), \
('923-NOW', 'Welcome to New Yorks new hit music channel,92.3 Now.', 'http://tx.sharp-stream.com/icecast.php?i=original106.mp3', 'http://www.923now.com', '923-NOW.png', 0, 0, 2, 1, '2011-11-11', 13952, 0, '2011-10-14 15:57:22', 1);";



const char *radio_station_setupsql44="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('181FM-Super-70s', '', 'http://listen.181fm.com/181-70s_128k.mp3', 'http://www.181.fm/', '181FM-Super-70s.png', 0, 0, 0, 1, '2011-11-11', 14040, 0, '2017-01-01 00:00:00', 1), \
('Slow-Jam', 'Une radio Slow Jam pour des France.', 'http://listen.radionomy.com/slow-jam', 'http://www.radionomy.com/en/radio/slow-jam', 'Slow-Jam.png', 0, 0, 0, 1, '2011-11-11', 14077, 0, '2017-01-01 00:00:00', 1), \
('DR P4 jylland', '', 'http://live-icy.gss.dr.dk:8000/A/A10H.mp3', 'http://www.dr.dk/p4/aarhus/', 'DR-P4-jylland-959.png', 0, 0, 5, 1, '2011-11-11', 14150, 0, '2011-10-14 16:08:24', 1), \
('Radio-Tunisie24', 'Among the stations Radio Tunisie24, users can discover Urban Radio Tunisie devoted to the Underground Music Rap Hip-Hop Rb.', 'http://tunisie24.net/radio-tunisie24-dance.m3u', 'http://www.tunisie24.net/', 'Radio-Tunisie24---Dance.png', 0, 0, 0, 1, '2011-11-11', 14259, 0, '2017-01-01 00:00:00', 0), \
('Box UK', '', 'https://uk7.internet-radio.com/proxy/danceradioukchatbox?mp=/live', 'https://www.danceradiouk.com/', 'Box-UK.png', 0, 0, 0, 1, '2011-11-11', 14263, 0, '2017-01-01 00:00:00', 1), \
('The Grooveradio', '', 'http://uk7.internet-radio.com:8352/listen.pls', 'http://thegrooveradio.co.uk/', 'thegrooveradio.png', 0, 0, 0, 1, '2011-11-11', 14263, 0, '2017-01-01 00:00:00', 1), \
('Danceattack', '', 'http://uk4.internet-radio.com:8049/1', 'http://www.danceattack.fm/', 'thegrooveradio.png', 0, 0, 0, 1, '2023-10-12', 14263, 0, '2017-01-01 00:00:00', 1), \
('Energy-98', '', 'http://listen.181fm.com/181-energy98_128k.mp3', 'http://www.energy981.com/', 'Energy-98.png', 0, 0, 0, 1, '2023-10-12', 14263, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql45="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Voivod', '', 'http://stream.laut.fm/eisradio', '', 'Voivod.png', 0, 0, 0, 1, '2011-11-11', 14271, 0, '2017-01-01 00:00:00', 1), \
('Radio-Deejay', '', 'http://live.radiodeejay.hr:7002', 'http://www.radiodeejay.hr/', 'Radio-Deejay.png', 0, 0, 0, 1, '2011-11-11', 14324, 0, '2017-01-01 00:00:00', 1), \
('Discofox-Radio', 'Hier gibt es Deutsche und Internationale Discofox-Scheiben.', 'http://stream.laut.fm/discofox', 'http://www.dj-willy.de/', 'Discofox-Radio.png', 0, 0, 0, 1, '2011-11-11', 14350, 0, '2017-01-01 00:00:00', 1), \
('Top40 pop', 'RadioMonster.FM - Tophits.', 'http://tophits.radiomonster.fm/320.mp3', '', '', 1, 320, 0, 1, '2011-11-11', 14350, 0, '2017-01-21 00:00:00', 1), \
('Record deep radio', '320k MP3', 'http://air.radiorecord.ru:805/deep_320', '', '', 1, 320, 0, 1, '2017-01-21', 14352, 0, '2017-01-21 00:00:00', 1), \
('LSD 25', 'LSD-25 Radio', 'http://a1.lsd-25.ru:8000/new', '', 'lsd25.png', 1, 192, 0, 1, '2017-01-21', 14353, 0, '2017-01-21 00:00:00', 1), \
('Radio nord', 'Radio Nord FM', 'http://radionord.radiostreaming.dk/RadioNord', '', 'radionord.png', 1, 128, 0, 1, '2023-11-05', 14354, 0, '2023-11-05 00:00:00', 1), \
('Nordjyske', '', 'http://stream.anr.dk/nordjyske', '', 'nordjyske.png', 1, 128, 0, 1, '2023-11-05', 14360, 0, '2023-11-05 00:00:00', 1), \
('radio-xmas', 'jul', 'http://www.radio-xmas.at:443/xmas', '', '', 1, 128, 0, 1, '2017-12-01', 14364, 0, '2017-12-01 00:00:00', 1), \
('HBR1.com - Tranceponder', 'Trance', 'http://ubuntu.hbr1.com:19800/trance.ogg', '', '', 1, 128, 0, 1, '2017-12-01', 14364, 0, '2014-06-11 00:00:00', 1), \
('HBR1.com -  Tronic Lounge','House', 'http://ubuntu.hbr1.com:19800/tronic.ogg', '', '', 1, 128, 0, 1, '2017-12-01', 14364, 0, '2014-06-11 00:00:00', 1), \
('jule radio', 'jul', 'http://tx-bauerdk.sharp-stream.com/http_live.php?i=395_dk_aacp&amsparams=playerid:SBS_RP_WEB;skey:1512330381;&awparams=loggedin:false', '', '', 1, 128, 0, 1, '2017-12-01', 14365, 0, '2017-12-01 00:00:00', 1), \
('Vegas Alternative', 'Vegas Alternative Rock', 'http://player.radio.com/listen/station/x-1075', '', '', 1, 128, 0, 1, '2017-12-01', 14366, 0, '2017-12-01 00:00:00', 1)";

// 700 -> 1000 is free on intnr



// ****************************************************************************************
//
// Create databaser/tables if now exist
//
// ****************************************************************************************

int create_radio_oversigt() {
    // mysql vars
//    int dbexist=0;
    MYSQL *conn=NULL;
    MYSQL_RES *res;
    MYSQL_ROW row;
//    MYSQL_ROW row;
    int mysqlerror;
    bool dbexist=false;
    printf("Check/Create mythtvcontroller db radio tables/database. \n");
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0);
    mysqlerror=mysql_errno(conn);
    if (mysqlerror) {
      if (conn==NULL) {
          conn=mysql_init(NULL);
          mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, NULL, 0, NULL, 0);
          mysqlerror=mysql_errno(conn);
      }
      if (conn) {
          mysql_query(conn,"CREATE DATABASE mythtvcontroller");                   // create radio stations db
          res = mysql_store_result(conn);
          printf("Create db %s \nMysql error %d %s\n",dbname,mysql_errno(conn),mysql_error(conn));
          mysqlerror=mysql_errno(conn);
      }
      if (mysqlerror==1044) {
          printf("No access to mysql database (mythtvcontroller) on host %s user %s mysql error code:%d \nPlease crate database mythtvcontroller \n",configmysqlhost,configmysqluser,mysql_errno(conn));
          exit(-1);
      }
      if ((dbname,mysql_errno(conn)!=2006) && (dbname,mysql_errno(conn)!=2003) && (dbname,mysql_errno(conn)!=1049)) {
          printf("Mysql error %s %s\n",dbname,mysql_error(conn));
          exit(-1);
      }
    }
    if ((mysqlerror==1049) || (mysqlerror==2006)) {
        // create db
        if (conn==NULL) conn=mysql_init(NULL);
        mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, "mysql", 0, NULL, 0);
        mysql_query(conn,"CREATE DATABASE IF NOT EXISTS mythtvcontroller");                   // create radio stations db
        res = mysql_store_result(conn);
        printf("Create db error %s \nMysql error %d \n",dbname,mysql_errno(conn));
        mysql_query(conn,"use mythtvcontroller");
        res = mysql_store_result(conn);
        printf("\nPlease update the config file mysql connection informations.\n");
        printf("Edit the file /etc/mythtv-ontroller.conf \nand update the mysql username and password settings.\n");
        if (conn) mysql_close(conn);
        exit(-1);
    }
    // connect
    if (conn) {
        mysql_query(conn,"select count(*) from 'mythtvcontroller.radio_stations'");
        res = mysql_store_result(conn);
        if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
                dbexist=true;
            }
        }
        if (!(dbexist)) {
            res = mysql_store_result(conn);
            mysql_query(conn,create_sql_radiostations);		  // create radio stations table
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , create_sql_radiostations);
              exit(1);
            }
            mysql_query(conn,create_sql_radio_countries);		// create country table
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , create_sql_radio_countries);
              exit(1);
            }
            mysql_query(conn,create_sql_radio_types);				// create types table
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , create_sql_radio_types);
              exit(1);
            }
            mysql_query(conn,create_sql_radio_types_data);	// create types data
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , create_sql_radio_types_data);
              exit(1);
            }

            mysql_query(conn,create_sql_lande_db);					// create lande data
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , create_sql_lande_db);
              exit(1);
            }


// create radio stations from here

            mysql_query(conn,radio_station_setupsql);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql4);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql4);
              exit(1);
            }

            mysql_query(conn,radio_station_setupsql8);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql8);
              exit(1);
            }

            mysql_query(conn,radio_station_setupsql13);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql13);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql17);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql17);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql27);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql27);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql31);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql31);
              exit(1);
            }

            mysql_query(conn,radio_station_setupsql39);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql39);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql40);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql40);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql41);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql41);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql42);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql42);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql43);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql43);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql44);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql44);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql45);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql45);
              exit(1);
            }
// end create radio stations
        }
        mysql_close(conn);
        return(1);
    } else {
        printf("Create tables error-. Mysql error %d \n",mysql_errno(conn));
        if (mysql_errno(conn)==1044) printf("MySQL Error: No access to %s database.\n",dbname);
        if (mysql_errno(conn)==1049) printf("MySQL Error: Unknown database %s\n",dbname);
        printf("Mysql error %s %s\n",dbname,mysql_error(conn));
        exit(1);
    }
    return(0);
}
