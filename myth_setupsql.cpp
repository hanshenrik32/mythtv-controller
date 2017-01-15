#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "myth_setupsql.h"

extern char *dbname;                                            // internal database name in mysql (music,movie,radio)

extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];


const char *create_sql_radiostations="CREATE TABLE IF NOT EXISTS `radio_stations` (`name` varchar(80) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`beskriv` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`stream_url` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`homepage` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`gfx_link` varchar(200) CHARACTER SET latin1 COLLATE latin1_danish_ci NOT NULL,`art` int(4) NOT NULL,`bitrate` int(11) NOT NULL,`popular` int(11) NOT NULL,`aktiv` int(1) NOT NULL,`createdate` date NOT NULL, `intnr` int(11) NOT NULL AUTO_INCREMENT,`landekode` int(11) NOT NULL, `lastplayed` datetime DEFAULT NULL,`online` tinyint(1) NOT NULL DEFAULT '1', PRIMARY KEY (`intnr`), KEY `art` (`art`), KEY `aktiv` (`aktiv`), KEY `popular` (`popular`), KEY `name` (`name`)) ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='rss radio stations'";
const char *create_sql_radio_countries="CREATE TABLE IF NOT EXISTS `radio_countries` (`land` varchar(80) NOT NULL,`landekode` int(11) NOT NULL,`imgpath` varchar(80) NOT NULL,PRIMARY KEY (`landekode`)) ENGINE=MyISAM DEFAULT CHARSET=latin1";
const char *create_sql_radio_types="CREATE TABLE IF NOT EXISTS `radiotypes` (`typename` varchar(80) NOT NULL, `art` int(2) NOT NULL, `sort` int(11) NOT NULL, PRIMARY KEY (`art`), KEY `sort` (`sort`)) ENGINE=MyISAM DEFAULT CHARSET=latin1";
const char *create_sql_radio_types_data="REPLACE INTO `radiotypes` (`typename`, `art`, `sort`) VALUES \
                                                                                    ('Rock/Classic Rock', 1, 5), \
                                                                                    ('Blues', 2, 2), \
                                                                                    ('All', 0, 0), \
                                                                                    ('Top 40', 40, 0), \
                                                                                    ('Dance/Techno', 4, 0), \
                                                                                    ('Jazz', 9, 0), \
                                                                                    ('Classical', 8, 5), \
                                                                                    ('Alternative', 7, 0), \
                                                                                    ('Country music', 6, 13), \
                                                                                    ('R&B/HipHop', 5, 7), \
                                                                                    ('Oldies', 3, 14), \
                                                                                    ('Trance', 41, 41), \
                                                                                    ('70''s music', 12, 15), \
                                                                                    ('80''s music', 13, 16), \
                                                                                    ('90''s music', 14, 17), \
                                                                                    ('HipHop', 15, 4), \
                                                                                    ('House', 16, 1), \
                                                                                    ('Most listened.', 19, 30), \
                                                                                    ('By artist.', 21, 31), \
                                                                                    ('Countries.', 27, 33), \
                                                                                    ('Bit rate.', 28, 34), \
                                                                                    ('Disco', 31, 36), \
                                                                                    ('Talk', 42, 42), \
                                                                                    ('Metal', 43, 43), \
                                                                                    ('Rap', 44, 44), \
                                                                                    ('Pop', 45, 45), \
                                                                                    ('Various', 46, 46), \
                                                                                    ('Funk', 47, 47), \
                                                                                    ('Reggae', 48, 48), \
                                                                                    ('Gospel', 49, 49), \
                                                                                    ('Techno', 50, 50), \
                                                                                    ('Folk', 51, 51), \
                                                                                    ('Mixed', 52, 52), \
                                                                                    ('Rhythm & Blues', 53, 53), \
                                                                                    ('Love songs', 54, 54),\
                                                                                    ('Electronic', 30, 30),\
                                                                                    ('Trance', 29, 29)";





const char *create_sql_lande_db="REPLACE INTO `radio_countries` (`land`, `landekode`, `imgpath`) VALUES \
                                                                ('Yugoslavia', 3, 'yu.jpg'), \
                                                                ('France',   4, 'fr.jpg'), \
                                                                ('Luxembourg', 5, 'luxembourg.jpg'), \
                                                                ('Holland',  6, 'nl.jpg'), \
                                                                ('Usa',      7, 'usa.jpg'), \
                                                                ('Tyskland', 8, 'de.jpg'), \
                                                                ('England',  9, 'uk.jpg'), \
                                                                ('Rusland', 10, 'ru.jpg'), \
                                                                ('Israel',  11, 'Israel.jpg'), \
                                                                ('Austria', 12, 'Austria.jpg'), \
                                                                ('Lebanon',  13, 'lebanon.jpg'), \
                                                                ('Latvia',  14, 'latvia.jpg'), \
                                                                ('Vietnam', 16, 'Vietnam.jpg'), \
                                                                ('Saudi-Arabia', 17, 'Saudi-Arabia.jpg'), \
                                                                ('as',      18, 'as.jpg'), \
                                                                ('brazil',  19, 'brazil.jpg'), \
                                                                ('Norge',   21, 'no.jpg'), \
                                                                ('Polen',   22, 'pl.jpg'), \
                                                                ('Sverige', 23, 'se.jpg'), \
                                                                ('Switzerland', 24, 'sw.jpg'), \
                                                                ('Mexico',  25, 'mexico.jpg'), \
                                                                ('Belgium', 26, 'be.jpg'), \
                                                                ('Canada',  27, 'ca.jpg'), \
                                                                ('Australia', 28, 'as.jpg'), \
                                                                ('Rusland', 29, 'ru.jpg'), \
                                                                ('Spain',   30, 'sp.jpg'), \
                                                                ('United Arab Emirates', 31, 'ae.jpg'), \
                                                                ('Hungary', 32, 'hu.jpg'), \
                                                                ('Thailand', 33, 'th.jpg'), \
                                                                ('Greece',  34, 'gr.jpg'), \
                                                                ('Bosnia and Herzegovina', 35, 'bk.jpg'), \
                                                                ('Bulgaria', 36, 'nu.jpg'), \
                                                                ('India', 37, 'in.jpg'), \
                                                                ('Portugal', 38, 'po.jpg'), \
                                                                ('Iran', 39, 'ir.jpg'), \
                                                                ('Korea', 40, 'ks.jpg'), \
                                                                ('Romania', 41, 'romania.jpg'), \
                                                                ('Italy',   42, 'it.jpg'), \
                                                                ('Finland', 43, 'fi.jpg'), \
                                                                ('Bahrain', 44, 'bahrain.jpg'), \
                                                                ('Denmark', 45, 'dk.jpg'), \
                                                                ('Chile', 46, 'chile.jpg'), \
                                                                ('Chile', 47, 'chile.jpg'), \
                                                                ('Slovakia', 48, 'Slovakia.jpg'), \
                                                                ('Ukraine', 49, 'Ukraine.jpg'), \
                                                                ('Hungary', 50, 'hu.jpg'), \
                                                                ('Colombia', 51, 'co.jpg'), \
                                                                ('Dominican Republic', 52, 'do.jpg'), \
                                                                ('Azerbaijan', 53, 'Azerbaijan.png'), \
                                                                ('Lithuania', 54, 'Lithuania.png'), \
                                                                ('Andorra', 55, 'Andorra.png'), \
                                                                ('Estonia', 56, 'Estonia.png'), \
                                                                ('Tajikistan', 57, 'Tajikistan.png'), \
                                                                ('Turkey', 58, 'Turkey.png'), \
                                                                ('Mongolia', 59, 'Mongolia.jpg'), \
                                                                ('Belarus', 60, 'Belarus.jpg'), \
                                                                ('Slovenia', 61, 'Slovenia.jpg'), \
                                                                ('Cyprus', 62, 'Cyprus.jpg'), \
                                                                ('China', 63, 'China.jpg'), \
                                                                ('Cambodia', 64, 'Cambodia.jpg'), \
                                                                ('Indonesia', 65, 'Indonesia.jpg'), \
                                                                ('Singapore', 66, 'Singapore.jpg'), \
                                                                ('Croatia', 67, 'Croatia.jpg'), \
                                                                ('Czech Republic', 68, 'Czech Republic.jpg')";

const char *radio_station_setupsql="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('P2 DK', 'Dansk radio', 'http://live-icy.gss.dr.dk:8000/A/A04H.mp3', 'www.dr.dk/P2', 'DR-P2-1023.png', 1, 320, 62, 1, '2011-07-28', 1, 45, '2011-09-15 22:20:34', 1), \
('P3 DK', 'www.p3.dk Dansk radio.', 'http://live-icy.gss.dr.dk:8000/A/A05H.mp3', 'www.dr.dk', 'DR-P3-939.png', 1, 128, 77, 1, '2011-07-28', 2, 45, '2011-10-20 15:37:44', 1), \
('P1 DK', 'Danish radio play clasisk music [Bitrate: 128]', 'http://live-icy.gss.dr.dk:8000/A/A03H.mp3', '', 'DR-P1-908.png', 8, 320, 63, 1, '2011-07-29', 4, 45, '2011-09-16 21:50:41', 1), \
('DR P7 Mix DK', '[Bitrate: 128]', 'http://live-icy.gss.dr.dk:8000/A/A21H.mp3', '', 'DR-P7-Mix.png', 40, 320, 64, 1, '2011-07-30', 6, 45, '2011-09-29 13:40:49', 1), \
('MAXXIMA', '', 'http://maxxima.mine.nu:8000/', '', '', 12, 0, 10, 1, '2011-08-02', 37, 4, '2011-10-02 02:25:35', 1), \
('Bay Radio', '[Bitrate: 128]', 'http://icy-e-01.sharp-stream.com/tcbay.mp3', 'http://www.swanseabayradio.com/', '', 0, 320, 1, 1, '2011-07-31', 7, 45, '2011-09-17 18:06:22', 1), \
('Sonic universe', 'Transcending the world of jazz with eclectic. [Bitrate: 128]', 'http://voxsc1.somafm.com:8600', '', '', 9, 128, 0, 1, '2011-07-31', 8, 7, NULL, 0), \
('Heat radio UK', '[Bitrate: 128] Denne radio er mega cool og spiller meget godt og blandet musik fra england. ', 'http://ams01.egihosting.com:9108/', '', 'Heat-Radio.png', 3, 128, 242, 1, '2011-07-31', 9, 9, '2011-10-21 15:23:52', 1),  \
('Radio 100', 'Med en god og varieret musikprofil, morsomme og intelligente værter samt en seriøs nyhedsformidling har Radio 100 leveret kvalitetsradio til de danske radiolyttere hele døgnet siden den 15. november 2', 'http://onair.100fmlive.dk/100fm_live.mp3', '', '', 14, 128, 43, 1, '2011-07-31', 11, 45, '2011-10-09 16:13:11', 1), \
('Radio Aura DK', 'DK', 'http://icecast.xstream.dk:80/aura', '', 'Radio-Aura-1054.png', 40, 128, 46, 1, '2011-08-02', 12, 45, '2011-10-11 23:05:02', 1), \
('181FM', '', 'http://uplink.181.fm:8068/', '', '181.fm.png', 0, 128, 1, 1, '2011-08-02', 15, 7, '2011-09-27 17:58:45', 1), \
('Energy 93 Eurodance', '', 'http://uplink.181.fm:8044/', '', '181.fm.png', 4, 0, 5, 1, '2011-08-02', 16, 7, '2011-10-07 16:02:00', 1), \
('Just Hiphop', '', 'http://stream.laut.fm:80/justhiphop', '', '', 5, 0, 2, 1, '2011-08-02', 17, 29, '2011-09-13 10:53:29', 1), \
('Groove Salad', 'A nicely chilled plate of ambient/downtempo beats and grooves.', 'http://ice1.somafm.com/groovesalad-128-aac', '', '', 16, 128, 13, 1, '2011-07-31', 19, 7, '2011-10-07 15:58:57', 1), \
('Radio EFM', 'France', 'http://radioefm.ice.infomaniak.ch:80/radioefm-high.mp3', '', '', 0, 128, 6, 1, '2011-08-01', 22, 4, NULL, 1), \
('The voice DK', 'The voice denmark.', 'http://stream.thevoicemobil.dk:80/pop64', '', '', 40, 64, 5, 1, '2011-08-01', 23, 45, '2011-09-11 00:08:02', 0), \
('Private Investigations', '', 'http://listen.radionomy.com/private-investigations', '', '', 21, 0, 0, 1, '2011-08-02', 25, 7, NULL, 1), \
('Capital 95.8', '', 'http://media-ice.musicradio.com:80/CapitalMP3', '', '', 0, 192, 2, 1, '2011-08-05', 39, 9, NULL, 1), \
('French Kiss FM', '', 'http://stream.frenchkissfm.com:80', '', '', 0, 192, 3, 1, '2011-08-05', 40, 4, NULL, 1), \
('TechnoBase.FM', '', 'http://listen.technobase.fm/tunein-dsl-pls', 'http://TechnoBase.FM', '', 50, 192, 1, 0, '2011-08-05', 43, 8, NULL, 1), \
('Hitparty fr', '', 'http://87.98.129.202:443', 'http://www.hitparty.fr/', 'hit-Party-Radio.png', 40, 192, 92, 1, '2011-08-05', 44, 4, '2011-10-18 18:33:39', 1), \
('Slow Radio', '(Love Songs & Easy Listening)', 'http://stream2.slowradio.com', 'http://en.slowradio.com/', '', 54, 192, 1, 1, '2011-08-05', 51, 4, NULL, 1), \
('AnimeNfo Radio', '[Bitrate: 192]', 'http://69.60.255.236:8000', '', '', 0, 192, 1, 1, '2011-08-10', 61, 27, '2011-09-01 22:42:06', 1), \
('Radio88 Szeged FM 95.4 ', '[Bitrate: 192]', 'http://88.151.99.22:8000', '', '', 0, 192, 0, 1, '2011-08-10', 74, 32, NULL, 1), \
('RADIOORANJE', 'www.radiooranje.nl bij verzoekjes  [Bitrate: 192] ', 'http://79.170.91.205:8000', '', '', 0, 192, 0, 1, '2011-08-10', 80, 21, NULL, 1), \
('Radio Sunshine ', '[Bitrate: 192] ', 'http://62.75.143.87:8000', '', '', 0, 192, 1, 1, '2011-08-10', 98, 42, NULL, 1), \
('Radio Hollandio [West Brabant]', '[Bitrate: 192]', 'http://62.212.132.9:8808', '', '', 0, 192, 3, 1, '2011-08-10', 101, 6, NULL, 1), \
('Beatdance FM', '[Bitrate: 192]', 'http://87.118.78.2:8050', '', '', 0, 192, 1, 1, '2011-08-10', 112, 25, '2011-10-03 19:18:51', 1), \
('RADIO MUNDUS', '[Bitrate: 192]', 'http://85.25.141.128:8010', '', '', 0, 192, 2, 1, '2011-08-10', 113, 9, '2011-09-15 18:30:33', 1), \
('Gold24 [trinity-32 relay]', '[Bitrate: 192]', 'http://85.195.17.209:8080', '', '', 8, 192, 0, 1, '2011-08-10', 123, 23, NULL, 1), \
('Kleine Piraat Internetradio', '[Bitrate: 192]', 'http://91.213.69.145:7030', '', '', 0, 192, 0, 1, '2011-08-10', 166, 6, NULL, 1), \
('BABELSBERG-HITRADIO.DE', '[Bitrate: 192]', 'http://46.163.75.84:8000', '', '', 40, 192, 1, 1, '2011-08-10', 171, 42, NULL, 1), \
('PIRATENFAMILIE', '[Bitrate: 192] ', 'http://46.231.87.20:8154', '', '', 0, 192, 0, 1, '2011-08-10', 192, 30, NULL, 1), \
('Radio 8FM Classics & Hits', '[Bitrate: 192]', 'http://46.231.87.21:8804', '', '', 0, 192, 0, 1, '2011-08-10', 196, 30, NULL, 1), \
('Singlestreff', '[Bitrate: 192]', 'http://87.118.119.128:10300', '', '', 46, 192, 0, 1, '2011-08-10', 199, 8, NULL, 1), \
('Ohrfunk.de Studio Marburg 3', '[Bitrate: 192]  ', 'http://46.4.77.21:8000', '', '', 1, 192, 0, 1, '2011-08-10', 214, 8, NULL, 1), \
('www.gezelligheidsradio.nl', '[Bitrate: 192]', 'http://178.20.171.32:8032', '', '', 0, 192, 0, 1, '2011-08-10', 217, 3, NULL, 1), \
('radio decadas', '[Bitrate: 192]', 'http://174.37.194.139:8316', '', '', 46, 192, 0, 1, '2011-08-10', 227, 7, NULL, 1), \
('Nashville FM', '[Bitrate: 192]', 'http://46.231.87.21:8300', '', '', 27, 192, 0, 1, '2011-08-10', 228, 30, NULL, 1), \
('Radio 8FM [West Brabant]', '[Bitrate: 192]  ', 'http://46.231.87.21:8418', '', '', 0, 192, 0, 1, '2011-08-10', 257, 30, NULL, 1), \
('Dance Fm Live', '[Bitrate: 192]', 'http://109.123.114.74:8006', '', '', 0, 192, 1, 1, '2011-08-10', 279, 31, '2011-10-17 23:43:04', 1), \
('Varia FM', 'Varia FM - Met Live Dj`s En Nonstop Muziek  [Bitrate: 192]', 'http://62.212.132.9:8704', '', '', 4, 192, 0, 1, '2011-08-10', 298, 6, NULL, 1), \
('ERDE Audio (www.Euer - Radio.DE)', '[Bitrate: 192]', 'http://188.165.201.141:9666', '', '', 0, 192, 0, 1, '2011-08-10', 331, 8, NULL, 1), \
('MuzsikusRadio', '[Bitrate: 192]  ', 'http://212.40.96.44:8011', '', '', 0, 192, 0, 1, '2011-08-10', 348, 50, NULL, 1), \
('NetBeatz BurnWeb', '[Bitrate: 192]', 'http://87.118.109.60:4444', '', '', 0, 192, 0, 1, '2011-08-10', 357, 8, NULL, 1), \
('Frequence Orange ', '[Bitrate: 192] ', 'http://94.23.213.103:8000', '', '', 0, 192, 0, 1, '2011-08-10', 358, 37, NULL, 1), \
('Radio Tunestorm ', '[Bitrate: 192]  ', 'http://213.239.196.116:8000', '', '', 0, 192, 0, 1, '2011-08-10', 359, 8, NULL, 1), \
('Radio Speedy Gemert', '[Bitrate: 192]  ', 'http://46.231.87.21:8312', '', '', 0, 192, 0, 1, '2011-08-10', 368, 30, NULL, 1), \
('AMSTERDAM STUDIO 7', '[Bitrate: 192]', 'http://62.212.132.9:8730', '', '', 46, 192, 0, 1, '2011-08-10', 385, 6, NULL, 1), \
('TOP 100 RADIO', 'EURO DANCE RADIO [Bitrate: 192]', 'http://88.198.3.15:8300', '', '', 4, 192, 4, 1, '2011-08-10', 392, 8, NULL, 1), \
('soe.one', '[Bitrate: 192]', 'http://178.63.103.214:8200/soeone', '', '', 0, 192, 0, 1, '2011-08-10', 403, 8, NULL, 1), \
('studio 3 cuijk', '[Bitrate: 192]  ', 'http://62.212.132.9:8408', '', '', 46, 192, 0, 1, '2011-08-10', 414, 6, NULL, 1), \
('Radio Zwiebel', '[Bitrate: 192]  ', 'http://80.237.208.92:20296', '', '', 0, 192, 0, 1, '2011-08-10', 416, 8, NULL, 1), \
('Radio Stand By', '[Bitrate: 192]   ', 'http://188.165.206.81:8010', '', '', 46, 192, 0, 1, '2011-08-10', 422, 33, NULL, 1), \
('MotorosHangRadio', '[Bitrate: 192] ', 'http://212.40.96.44:8006', '', '', 1, 192, 0, 1, '2011-08-10', 423, 32, NULL, 1), \
('GROOVE 95 ', '*** GROOVE95.COM FROM THE NETHERLANDS NOW DJ TRISTANO :-) *** [Bitrate: 192]  ', 'http://82.94.166.98:80', '', '', 0, 192, 0, 1, '2011-08-10', 424, 6, NULL, 1), \
('Radio Delta [www.radiodelta.nl', '24/7 [Bitrate: 192]   ', 'http://62.212.132.9:8574', '', '', 0, 192, 0, 1, '2011-08-10', 444, 6, NULL, 1), \
('RadioZapliN', '[Bitrate: 192]   ', 'http://91.209.128.139:8000', '', '', 0, 192, 0, 1, '2011-08-10', 454, 29, NULL, 1), \
('Radio Temps Rodez', '[Bitrate: 192]', 'http://213.186.45.173:8041', '', '', 46, 192, 0, 1, '2011-08-10', 480, 4, NULL, 1), \
('KDHR', '[Bitrate: 192]  ', 'http://155.135.55.205:8000', '', '', 46, 192, 1, 1, '2011-08-10', 483, 7, NULL, 1), \
('RADIO.IPIP.CZ', '[Bitrate: 192]   ', 'http://212.96.160.160:8006', '', '', 46, 192, 0, 1, '2011-08-10', 504, 68, NULL, 1), \
('RADIO ROKKO', '[Bitrate: 192]  ', 'http://173.192.205.189:80', '', '', 0, 192, 0, 1, '2011-08-10', 505, 7, NULL, 1), \
('radiokiss', ' ', 'http://124.53.158.153:8000', '', '', 0, 192, 0, 1, '2011-08-10', 508, 27, NULL, 1), \
('4TH FLOOR HitRadio', '[Bitrate: 192]   ', 'http://178.20.171.32:8022', '', '', 40, 192, 0, 1, '2011-08-10', 525, 3, NULL, 1), \
('hollandiafm piraten hits ', '[Bitrate: 192]', 'http://178.20.171.32:8046', '', '', 0, 192, 0, 1, '2011-08-10', 553, 3, NULL, 1), \
('Proxima FM ', '[Bitrate: 192]  ', 'http://91.187.93.115:8000', '', '', 0, 192, 0, 1, '2011-08-10', 557, 55, NULL, 1), \
('GRIMALDIFM', '[Bitrate: 192]  ', 'http://91.121.29.128:8058', '', '', 0, 192, 0, 1, '2011-08-10', 603, 4, NULL, 1), \
('NB Radio', '[Bitrate: 192]  ', 'http://212.62.227.4:7777', '', '', 46, 192, 0, 1, '2011-08-10', 641, 21, NULL, 1), \
('RTV Meppel ', '[Bitrate: 192]   ', 'http://178.20.171.32:8030', '', '', 46, 192, 0, 1, '2011-08-10', 687, 3, NULL, 1), \
('3Sinhala Radio Network', '[Bitrate: 128]  ', 'http://66.90.103.189:8888', '', '', 0, 128, 0, 1, '2011-08-11', 1054, 7, NULL, 1), \
('Chroma Classic Rock', '[Bitrate: 128]', 'http://chromaradio.com:8010/', '', '', 1, 128, 0, 1, '2011-08-11', 1061, 8, NULL, 1), \
('Pure Jazz Radio', '', 'http://71.125.12.37:8000', '', '', 9, 128, 0, 1, '2011-08-11', 1064, 7, NULL, 1), \
('Party Central Radio', '[Bitrate: 128]', 'http://208.85.240.210:80', '', '', 0, 128, 0, 1, '2011-08-11', 1067, 7, NULL, 1), \
('1980s.FM', '[Bitrate: 128]', 'http://209.9.229.214:80', '', '', 0, 128, 0, 1, '2011-08-11', 1086, 7, NULL, 1), \
('KEK DUNA ', '[Bitrate: 128] ', 'http://212.92.23.237:8010', '', '', 45, 128, 0, 1, '2011-08-11', 1096, 42, NULL, 1), \
('GuggliFox (Stream)', '[Bitrate: 128]  ', 'http://46.4.77.21:27160', '', '', 0, 128, 0, 1, '2011-08-11', 1101, 8, NULL, 1), \
('ANDYS 80S', '[Bitrate: 128]  ', 'http://209.9.238.4:9844', '', '', 0, 128, 0, 1, '2011-08-11', 1105, 7, NULL, 1), \
('Radio Free Phoenix', '[Bitrate: 128]  ', 'http://68.178.199.78:8000', '', '', 0, 128, 0, 1, '2011-08-11', 1114, 7, NULL, 1), \
('SOULPOWERfm', '101-va-motown_summer_mix  [Bitrate: 128]  ', 'http://91.143.83.37:12000', '', '', 0, 128, 0, 1, '2011-08-11', 1153, 8, NULL, 1), \
('RadioMusic EasyAndItaly', '[Bitrate: 128]  ', 'http://188.165.206.81:8110', '', '', 0, 128, 0, 1, '2011-08-11', 1166, 33, NULL, 1), \
('Old Rock Radio', '[Bitrate: 128]', 'http://173.236.29.52:80', '', '', 1, 128, 0, 1, '2011-08-11', 1167, 7, NULL, 1), \
('RadioGadangme', '[Bitrate: 128]  ', 'http://38.96.148.138:8670', '', '', 0, 128, 0, 1, '2011-08-11', 1182, 7, NULL, 1), \
('Rewound Radio', '[Bitrate: 128]  ', 'http://71.125.37.66:9000', '', '', 3, 128, 0, 1, '2011-08-11', 1192, 0, NULL, 1), \
('TOP FM Beograd 106', '', 'http://109.206.96.12:8000', '', '', 0, 128, 0, 1, '2011-08-11', 1197, 8, NULL, 1), \
('Absolute Radio', '', 'http://network.absoluteradio.co.uk/core/audio/mp3/live.pls?service=a0hq', '', '', 14, 192, 3, 1, '2011-08-13', 1209, 45, '2011-10-07 16:02:04', 1), \
('Volldampfradio.com  -Anklicken', ' Reinhren', 'http://88.198.195.229:4010', '', '', 0, 128, 0, 1, '2011-08-11', 1218, 8, NULL, 1), \
('iglesiabautista', 'iglesiabautista.org [Bitrate: 128]', 'http://174.133.52.163:8128', '', '', 0, 128, 0, 1, '2011-08-11', 1221, 7, NULL, 1), \
('VIBES RADIO STATION', '[Bitrate: 128]  ', 'http://195.24.54.14:8000', '', '', 16, 128, 3, 1, '2011-08-11', 1229, 36, '2011-09-06 17:30:27', 1), \
('Radio 80', '[Bitrate: 128]  ', 'http://94.23.2.73:7020', '', '', 13, 128, 0, 1, '2011-08-11', 1232, 4, NULL, 1), \
('Radio Free Americana', '[Bitrate: 128]  ', 'http://212.48.125.67:8000', '', '', 0, 128, 0, 1, '2011-08-11', 1260, 8, NULL, 1), \
('Radio Twilight', 'Live vanuit Heusden met DJ Jo en Coba Flamingo  [Bitrate: 128]  ', 'http://81.18.165.234:8000', 'www.radio-twilight.nl', '', 0, 128, 0, 1, '2011-08-11', 1282, 6, NULL, 1), \
('A State Of Trance.com.ua', '[Bitrate: 128]  ', 'http://89.252.2.4:8000', '', '', 41, 128, 1, 1, '2011-08-11', 1300, 49, NULL, 1), \
('Radio GARDARICA', 'Russia S-Petersburg', 'http://195.182.132.18:8260', '', '', 0, 128, 0, 1, '2011-08-11', 1316, 29, NULL, 1), \
('Memoryradio 2', '[Bitrate: 128]  ', 'http://188.165.247.175:5000', '', '', 0, 128, 0, 1, '2011-08-11', 1331, 4, NULL, 1), \
('KCSN Classical 88.5 HD2 - from Cal. State', ' Northridge ', 'http://130.166.82.14:8002', '', '', 8, 128, 0, 1, '2011-08-11', 1347, 7, NULL, 1), \
('palmenstrandradio.com', '[Bitrate: 128]  ', 'http://188.138.16.171:8000', '', '', 0, 128, 0, 1, '2011-08-11', 1400, 8, NULL, 1), \
('FunFM', '[Bitrate: 128]  ', 'http://91.82.85.44:9408', '', '', 45, 128, 1, 1, '2011-08-11', 1429, 50, '2011-08-28 17:55:55', 1), \
('Trancemission.FM Radio', '[Bitrate: 128]', 'http://87.230.84.182:6660', '', '', 0, 128, 0, 1, '2011-08-11', 1432, 8, NULL, 1), \
('RADIO POOL ', '[Bitrate: 128]  ', 'http://208.115.213.242:8010', '', '', 0, 128, 0, 1, '2011-08-11', 1441, 7, NULL, 1), \
('PAUL IN RIO RADIO: BRASIL', ' BRAZIL', 'http://69.175.13.131:8280', '', '', 0, 128, 0, 1, '2011-08-11', 1450, 7, NULL, 1), \
('Radio Mi Amigo', ' Live vanaf de Spaanse Costa ', 'http://46.231.87.21:8200', '', '', 0, 128, 0, 1, '2011-08-11', 1462, 30, NULL, 1), \
('Radio Kosova 91.9', '[Bitrate: 128]  ', 'http://82.114.72.2:8088', '', '', 0, 128, 0, 1, '2011-08-11', 1485, 7, NULL, 1), \
('Desi Music Mix', 'Punjabi music', 'http://66.23.234.242:8012', '', '', 0, 128, 0, 1, '2011-08-11', 1492, 7, NULL, 1), \
('Trancemission.FM Radio', '[Bitrate: 128]  ', 'http://87.230.21.161:6667', '', '', 0, 128, 0, 1, '2011-08-11', 1534, 12, NULL, 1), \
('Discover Trance Radio', 'TranceAction 022', 'http://195.12.29.146:8010', '', '', 41, 128, 2, 1, '2011-08-11', 1554, 9, '2011-10-07 16:02:13', 1), \
('KPOP @ Big B Radio', '[Bitrate: 128]', 'http://184.95.62.170:9002', '', '', 0, 128, 0, 1, '2011-08-11', 1572, 7, NULL, 1), \
('Star FM - Rock Classics', '[Bitrate: 128] ', 'http://87.230.53.43:8008', '', '', 0, 128, 0, 1, '2011-08-11', 1617, 12, NULL, 1), \
('Bienvenue', '[Bitrate: 128]  ', 'http://188.165.32.218:9042', '', '', 0, 128, 0, 1, '2011-08-11', 1623, 26, NULL, 1), \
('~BlackSection.FM', '[Bitrate: 128] ', 'http://89.31.134.145:80', '', '', 0, 128, 0, 1, '2011-08-11', 1648, 12, NULL, 1), \
('swissradio Modern Jazz', '[Bitrate: 128]  ', 'http://82.197.165.140:80', '', '', 9, 128, 0, 1, '2011-08-11', 1651, 24, NULL, 1), \
('RadioMusic - RadioSals4te', '[Bitrate: 128]  ', 'http://188.165.206.81:8100', '', '', 0, 128, 0, 1, '2011-08-11', 1689, 33, NULL, 1), \
('Radio MRDJ', '[Bitrate: 128]  ', 'http://212.18.63.51:9000', '', '', 4, 128, 0, 1, '2011-08-11', 1693, 61, NULL, 1), \
('TheBlast.FM: Christian Rock', '[Bitrate: 128]  ', 'http://199.58.164.238:80', '', '', 0, 128, 0, 1, '2011-08-11', 1696, 7, NULL, 1), \
('Public Domain', '[Bitrate: 128] [Max Listeners: 900]', 'http://82.197.165.137:80', '', '', 0, 128, 0, 1, '2011-08-11', 1710, 24, NULL, 1), \
('Radio Nova', '[Bitrate: 128]  ', 'http://85.159.16.93:80', '', '', 1, 128, 0, 1, '2011-08-11', 1717, 18, NULL, 1);";


const char *radio_station_setupsql4="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('HIPER FM // AGARRA A TUA MUSICA!', '[Bitrate: 128]', 'http://109.71.41.250:8158', '', '', 40, 128, 1, 1, '2011-08-11', 1757, 38, NULL, 1), \
('Star FM NBG', '[Bitrate: 128]', 'http://87.230.53.43:8004', '', '', 1, 128, 0, 1, '2011-08-11', 1759, 12, NULL, 1), \
('Munichs Hardest Hits', 'elodic Rock & AOR', 'http://84.19.188.7:8010', '', '', 40, 128, 0, 1, '2011-08-11', 1774, 8, NULL, 1), \
('From hell', '[Bitrate: 128]', 'http://87.230.53.43:7000', '', '', 1, 128, 0, 1, '2011-08-11', 1775, 12, NULL, 1), \
('Alouette', '[Bitrate: 128]', 'http://91.197.138.3:80', '', '', 45, 128, 0, 1, '2011-08-11', 1778, 4, NULL, 1), \
('Japan-A-Radio', '[Bitrate: 128]', 'http://64.62.194.40:9010', '', '', 0, 128, 0, 1, '2011-08-11', 1780, 7, NULL, 1), \
('Split Infinity Radio', '[Bitrate: 128]', 'http://46.4.120.170:80', '', '', 0, 128, 0, 1, '2011-08-11', 1789, 8, NULL, 1), \
('Renessans 94.2', '[Bitrate: 128]', 'http://217.20.164.163:8014', '', '', 9, 128, 0, 1, '2011-08-11', 1806, 49, NULL, 1), \
('Antena Radio', 'chillchannel [Bitrate: 128]  ', 'http://94.23.38.225:7005', '', '', 0, 128, 0, 1, '2011-08-11', 1810, 4, NULL, 1), \
('psyradio*fm', '[Bitrate: 128]', 'http://81.88.36.42:8020', '', '', 0, 128, 0, 1, '2011-08-11', 1812, 8, NULL, 1), \
('Radio Beograd 1', '[Bitrate: 128]', 'http://195.252.107.194:8002', '', '', 0, 128, 0, 1, '2011-08-11', 1818, 3, NULL, 1), \
('EBM Radio', 'Invincible Limit', 'http://87.106.138.241:7000', '', '', 0, 128, 0, 1, '2011-08-11', 1820, 7, NULL, 1), \
('HAPPYDAY', '[Bitrate: 128]  ', 'http://222.122.131.69:8000', '', '', 0, 128, 1, 1, '2011-08-11', 1832, 40, NULL, 1), \
('Springsteenradio.com', '[Bitrate: 128]   ', 'http://67.212.166.178:9000', '', '', 21, 128, 0, 1, '2011-08-11', 1884, 7, NULL, 1), \
('Swiss Internet Radio', '', 'http://82.197.167.139:80', '', '', 0, 128, 0, 1, '2011-08-11', 1889, 23, NULL, 1), \
('psyradio*fm-psychannel', '[Bitrate: 128] [Max Listeners: 410]', 'http://81.88.36.44:8030', '', '', 0, 128, 0, 1, '2011-08-11', 1896, 8, NULL, 1), \
('WKSU Classical', 'WKSU Classical: Ludwig van Beethoven - Piano Sonata No. 31  [Bitrate: 128] [Max Listeners: 2000]', 'http://66.225.205.8:8030', '', '', 8, 128, 0, 1, '2011-08-11', 1910, 7, NULL, 1), \
('SLOW JAMZ For Lovers', '[Bitrate: 128] ', 'http://85.25.16.7:8028', '', '', 0, 128, 0, 1, '2011-08-11', 1925, 8, NULL, 1), \
('Scansat', 'Scansat Trondheim  [Bitrate: 128]  ', 'http://212.62.227.4:2049', '', '', 0, 128, 0, 1, '2011-08-11', 1926, 21, NULL, 1), \
('DeepFM', '(DeepFM) [Bitrate: 128] ', 'http://195.66.134.40:80', '', '', 0, 128, 1, 1, '2011-08-11', 1954, 24, NULL, 1), \
('memoryradio', '[Bitrate: 128]  ', 'http://188.165.247.175:4000', '', '', 0, 128, 0, 1, '2011-08-11', 1957, 4, NULL, 1), \
('Deep Mix Moscow Radio', '[Bitrate: 128] ', 'http://85.21.79.31:7128', '', '', 0, 128, 0, 1, '2011-08-11', 1962, 10, NULL, 1), \
('4U Classic Rock', 'Classic Rock Station  [Bitrate: 128]  ', 'http://217.114.200.100:80', '', '', 0, 128, 0, 1, '2011-08-11', 1964, 29, NULL, 1), \
('DJFM Ukraine', '[Bitrate: 128] [Max Listeners: 800]', 'http://217.20.164.163:8010', '', '', 0, 128, 0, 1, '2011-08-11', 1974, 49, NULL, 1), \
('Kalasam.com', '[Bitrate: 128]', 'http://173.192.207.51:8084', '', '', 0, 128, 0, 1, '2011-08-11', 1984, 7, NULL, 1), \
('NAXI RADIO 96', '9MHz - Beograd - www.naxi.rs ', 'http://193.243.169.34:9150', '', '', 0, 128, 0, 1, '2011-08-11', 2009, 45, NULL, 1), \
('Classical 96.3FM CFMZ', '[Bitrate: 128] ', 'http://199.27.180.216:8017', '', '', 8, 128, 0, 1, '2011-08-11', 2043, 28, NULL, 1), \
('AnimeRadio.SU', '[Bitrate: 128]  ', 'http://78.46.91.38:8000', '', '', 0, 128, 0, 1, '2011-08-11', 2073, 8, NULL, 1), \
('Public Domain Jazz', '[Bitrate: 128] ', 'http://82.197.165.138:80', '', '', 0, 128, 0, 1, '2011-08-11', 2085, 24, NULL, 1), \
('COUNTRY 108', '[Bitrate: 128]   ', 'http://80.237.210.44:80', '', '', 6, 128, 0, 1, '2011-08-11', 2090, 8, NULL, 1), \
('Shanson 101.9 Kiev', '[Bitrate: 128] ', 'http://217.20.164.163:8002', '', '', 0, 128, 0, 1, '2011-08-11', 2091, 49, NULL, 1), \
('STAR FM Bln', '[Bitrate: 128] ', 'http://87.230.53.43:8000', '', '', 1, 128, 0, 1, '2011-08-11', 2092, 12, NULL, 1), \
('psyradio*fm', '[Bitrate: 128]', 'http://81.88.36.42:8010', '', '', 0, 128, 0, 1, '2011-08-11', 2098, 8, NULL, 1), \
('Raggakings the 24-7', 'Bersenbrueck ReggaeJam 2011 - Ranking Joe [Bitrate: 128]', 'http://64.202.98.51:7970', '', '', 0, 128, 0, 1, '2011-08-11', 2123, 7, NULL, 1), \
('Sharmanka 104.0 Kiev', '[Bitrate: 128]', 'http://217.20.164.163:8006', '', '', 0, 128, 0, 1, '2011-08-11', 2151, 49, NULL, 1), \
('LOLLIRADIO ITALIA ', '[Bitrate: 128]', 'http://94.23.67.172:8010', '', '', 0, 128, 0, 1, '2011-08-11', 2155, 26, NULL, 1), \
('RADIODISCOPOLO.FM', ' radio ZET', 'http://94.23.36.107:443', '', '', 0, 128, 0, 1, '2011-08-11', 2172, 4, NULL, 1), \
('HouseSection.FM', ' Austria House Music Radio - MusicBase.FM ~ [Bitrate: 128]', 'http://89.31.134.100:84', '', '', 16, 128, 0, 1, '2011-08-11', 2173, 12, NULL, 1), \
('CLASSICAL 102', '[Bitrate: 128]', 'http://80.237.210.39:80', '', '', 8, 128, 0, 1, '2011-08-11', 2176, 8, NULL, 1), \
('Beatles Radio.com', 'Solos', 'http://64.40.99.2:8088', '', '', 21, 128, 0, 1, '2011-08-11', 2187, 27, NULL, 1), \
('RadioMv.com', '[Bitrate: 128]', 'http://65.19.173.132:2204', '', '', 0, 128, 0, 1, '2011-08-11', 2190, 7, NULL, 1), \
('RMNradio....Music Fun and More!', '[Bitrate: 128]', 'http://80.237.201.92:8010', '', '', 0, 128, 0, 1, '2011-08-11', 2192, 8, NULL, 1), \
('La X Estereo', '[Bitrate: 128]', 'http://80.237.211.85:80', '', '', 0, 128, 0, 1, '2011-08-11', 2194, 8, NULL, 1), \
('HIT104', '[Bitrate: 128]', 'http://80.237.210.57:80', '', '', 40, 128, 0, 1, '2011-08-11', 2195, 8, NULL, 1), \
('Venice Classic', '[Bitrate: 128]  ', 'http://174.36.206.197:8000', '', '', 8, 128, 0, 1, '2011-08-11', 2200, 58, NULL, 1), \
('Radio Paloma - 100% Deutscher Schlager!', '[Bitrate: 128]', 'http://80.237.184.23:80', '', '', 45, 128, 0, 1, '2011-08-11', 2205, 8, NULL, 1), \
('DEFJAY.COM', '[Bitrate: 128] ', 'http://83.169.60.45:80', '', '', 0, 128, 0, 1, '2011-08-11', 2224, 28, NULL, 1), \
('Schlagerhoelle', '[Bitrate: 128]', 'http://84.19.184.27:30842', '', '', 0, 128, 0, 1, '2011-08-11', 2228, 8, NULL, 1), \
('DR P7 MIX', '', 'http://live-icy.gss.dr.dk:8000/A/A21H.mp3', '', '', 0, 128, 1, 1, '2011-08-13', 2241, 45, NULL, 1), \
('DeepFM', 'House Dance', 'http://195.66.134.41:80', '', '', 16, 320, 8, 1, '2011-08-16', 2270, 24, '2011-10-06 16:56:33', 1), \
('1Mix Radio', 'Trance', 'http://94.23.209.225:8018', '', '', 41, 320, 0, 1, '2011-08-16', 2380, 37, NULL, 1), \
('WKAZ-680', '', 'http://www.visma.ua/vismaaudio/visma.m3u', '', 'WKAZ-680.png', 0, 0, 0, 1, '2011-11-11', 2555, 0, '2017-01-01 00:00:00', 1), \
('Electro-Radio', '24h nonStop Electro House Music', 'http://stream.electroradio.ch:26630/', 'http://www.electroradio.ch/', 'Electro-Radio.png', 0, 0, 0, 1, '2011-11-11', 2564, 0, '2017-01-01 00:00:00', 1), \
('NERadio-International', 'The Best of Techno and Trance !!!', 'http://listen.neradio.fm/listen.pls', 'http://www.neradio.fm/', 'NERadio-International.png', 0, 0, 0, 1, '2011-11-11', 2567, 0, '2017-01-01 00:00:00', 1), \
('WEOS-897', '', 'http://www.publicbroadcasting.net/weos/ppr/weos.m3u', 'http://www.weos.org/', 'WEOS-897.png', 0, 0, 0, 1, '2011-11-11', 2568, 0, '2017-01-01 00:00:00', 1), \
('Ricky-Dillard--New-G', '', 'http://listen.neradio.fm/listen.pls', '', 'Ricky-Dillard--New-G.png', 0, 0, 0, 1, '2011-11-11', 2569, 0, '2017-01-01 00:00:00', 1), \
('Supreme-Radio', 'lo spazio per gli amanti di musica hip hop, r&amp;b, dancehall,reggaeton, soul, funk e afro.', 'http://supreme.newradiostream.com:8020', 'http://www.supremeradio.it/', 'Supreme-Radio.png', 0, 0, 0, 1, '2011-11-11', 2577, 0, '2017-01-01 00:00:00', 1), \
('From hell', '[Bitrate: 128]', 'http://87.230.53.43:7000', '', '', 1, 128, 0, 1, '2011-08-11', 2589, 12, NULL, 1), \
('MIX-96-961', 'MIX 96 is an Adult Contemporary station based in Sacramento, CA.  MIX 96 can be heard at radio.com', 'http://134.161.217.60:8000/listen.pls', 'http://www.kymx.com/', 'MIX-96-961.png', 0, 0, 0, 1, '2011-11-11', 2590, 0, '2017-01-01 00:00:00', 0), \
('Munichs Hardest Hits', 'elodic Rock & AOR', 'http://84.19.188.7:8010', '', '', 40, 128, 0, 1, '2011-08-11', 2594, 8, NULL, 1), \
('CrackBerrycom-Podcast', '', 'http://cp2.digi-stream.com:9094', '', 'CrackBerrycom-Podcast.png', 0, 0, 0, 1, '2011-11-11', 2597, 0, '2017-01-01 00:00:00', 1), \
('Captain--Tennille', '', 'http://cp2.digi-stream.com:9094', '', 'Captain--Tennille.png', 0, 0, 0, 1, '2011-11-11', 2598, 0, '2017-01-01 00:00:00', 1), \
('Radio-Nostalgia', '', 'http://listen.radionomy.com/radio-nostalgia', 'http://www.radio-nostalgia.nl/', 'Radio-Nostalgia.png', 0, 0, 0, 1, '2011-11-11', 2605, 0, '2017-01-01 00:00:00', 1), \
('Antenne-Bayern-Top-40', '', 'http://www.antenne.de/webradio/channels/top-40.aac.pls', 'http://www.antenne.de/', 'Antenne-Bayern-Top-40.png', 0, 0, 0, 1, '2011-11-11', 2617, 0, '2017-01-01 00:00:00', 1), \
('Marco-Masini', '', 'http://www.antenne.de/webradio/channels/top-40.aac.pls', '', 'Marco-Masini.png', 0, 0, 0, 1, '2011-11-11', 2619, 0, '2017-01-01 00:00:00', 1), \
('Star-1045', '', 'http://www.antenne.de/webradio/channels/top-40.aac.pls', 'http://www.104star.com/', 'Star-1045.png', 0, 0, 0, 1, '2011-11-11', 2621, 0, '2017-01-01 00:00:00', 1), \
('Belinda-Carlisle', '', 'http://www.antenne.de/webradio/channels/top-40.aac.pls', '', 'Belinda-Carlisle.png', 0, 0, 0, 1, '2011-11-11', 2622, 0, '2017-01-01 00:00:00', 1), \
('Vanessa-Williams', '', 'http://www.antenne.de/webradio/channels/top-40.aac.pls', '', 'Vanessa-Williams.png', 0, 0, 0, 1, '2011-11-11', 2623, 0, '2017-01-01 00:00:00', 1), \
('TEEZ-FM', '', 'http://listen.radionomy.com/teez-fm', 'http://teez-fm.blogspot.com/', 'TEEZ-FM.png', 0, 0, 0, 1, '2011-11-11', 2673, 0, '2017-01-01 00:00:00', 1), \
('Xpression-FM-877', 'The student radio station of the University of Exeter', 'http://xmedia.ex.ac.uk:8000/high-quality', 'http://www.xpressionfm.com', 'Xpression-FM-877.png', 0, 0, 0, 1, '2011-11-11', 2684, 0, '2017-01-01 00:00:00', 1), \
('Best-of-the-Week', '', 'http://xmedia.ex.ac.uk:8000/high-quality', '', 'Best-of-the-Week.png', 0, 0, 0, 1, '2011-11-11', 2685, 0, '2017-01-01 00:00:00', 1), \
('Family-Gospel-Music-Hour', '', 'http://asx.abacast.com/810inc-wedoam-64.pls', '', 'Family-Gospel-Music-Hour.png', 0, 0, 0, 1, '2011-11-11', 2706, 0, '2017-01-01 00:00:00', 1), \
('VRT-Studio-Brussel-1006', '', 'http://mp3.streampower.be/stubru-high.mp3', 'http://www.stubru.be/home', 'VRT-Studio-Brussel-1006.png', 0, 0, 2, 1, '2011-11-11', 2707, 0, '2011-10-14 15:50:22', 1), \
('Living-on-the-Edge', '', 'http://mp3.streampower.be/stubru-high.mp3', '', 'Living-on-the-Edge.png', 0, 0, 0, 1, '2011-11-11', 2708, 0, '2017-01-01 00:00:00', 1), \
('Hot-969', '', 'http://mp3.streampower.be/stubru-high.mp3', 'http://www.hot969.com', 'Hot-969.png', 0, 0, 0, 1, '2011-11-11', 2709, 0, '2017-01-01 00:00:00', 1), \
('96-Rock-965', '', 'http://mp3.streampower.be/stubru-high.mp3', 'http://whtq.com/', '96-Rock-965.png', 0, 0, 0, 1, '2011-11-11', 2710, 0, '2017-01-01 00:00:00', 1), \
('KRMS-1150', '', 'http://abc.net.au/res/streaming/audio/aac/local_perth.pls', 'http://www.1150krms.com/', 'KRMS-1150.png', 0, 0, 0, 1, '2011-11-11', 2718, 0, '2017-01-01 00:00:00', 1), \
('The-SOS-Band', '', 'http://abc.net.au/res/streaming/audio/aac/local_perth.pls', '', 'The-SOS-Band.png', 0, 0, 0, 1, '2011-11-11', 2719, 0, '2017-01-01 00:00:00', 1), \
('WPAD-1560', '', 'http://abc.net.au/res/streaming/audio/aac/local_perth.pls', '', 'WPAD-1560.png', 0, 0, 0, 1, '2011-11-11', 2721, 0, '2017-01-01 00:00:00', 1), \
('M-Ward', '', 'http://abc.net.au/res/streaming/audio/aac/local_perth.pls', '', 'M-Ward.png', 0, 0, 0, 1, '2011-11-11', 2722, 0, '2017-01-01 00:00:00', 1), \
('Sunday-Morning-Roundtable', '', 'http://www.181.fm/stream/asx/181-acidjazz', '', 'Sunday-Morning-Roundtable.png', 0, 0, 0, 1, '2011-11-11', 2730, 0, '2017-01-01 00:00:00', 1), \
('Indie-1031', 'We are LA &amp; the OC&#39;s independent internet radio station featuring Barely Legal Radio with Joe Escalante, Liz Warner &amp; Tedd Roman Mid-Days, Breakfast With The Smiths with Jose Maldonado, Co', 'http://www.181.fm/stream/asx/181-acidjazz', 'http://www.indie1031.com/', 'Indie-1031.png', 0, 0, 0, 1, '2011-11-11', 2731, 0, '2017-01-01 00:00:00', 1), \
('Magic-1051', '', 'http://www.181.fm/stream/asx/181-acidjazz', 'http://www.detroitmagic.com/', 'Magic-1051.png', 0, 0, 0, 1, '2011-11-11', 2732, 0, '2017-01-01 00:00:00', 1), \
('KQNA-1130', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.kqna.com/', 'KQNA-1130.png', 0, 0, 0, 1, '2011-11-11', 2757, 0, '2017-01-01 00:00:00', 1), \
('XL93-929', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.xl93.com/', 'XL93-929.png', 0, 0, 0, 1, '2011-11-11', 2758, 0, '2017-01-01 00:00:00', 1), \
('1077-FM-Music-For-Life', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.1077musicforlife.com/', '1077-FM-Music-For-Life.png', 0, 0, 0, 1, '2011-11-11', 2759, 0, '2017-01-01 00:00:00', 1), \
('ESPN-1240', '', 'http://stream.amellus.com:8000/KQNA.mp3', 'http://www.star94radio.com/ESPN1240.php', 'ESPN-1240.png', 0, 0, 0, 1, '2011-11-11', 2760, 0, '2017-01-01 00:00:00', 1), \
('Selena-Gomez--The-Scene', '', 'http://stream.amellus.com:8000/KQNA.mp3', '', 'Selena-Gomez--The-Scene.png', 0, 0, 0, 1, '2011-11-11', 2761, 0, '2017-01-01 00:00:00', 1), \
('Chet-Baker', '', 'http://stream.amellus.com:8000/KQNA.mp3', '', 'Chet-Baker.png', 0, 0, 0, 1, '2011-11-11', 2762, 0, '2017-01-01 00:00:00', 1), \
('KTHO-590', 'Lake Tahoe&#39;s Local Radio Station, with longtime local people.', 'http://s7.viastreaming.net:7540', 'http://www.kthoradio.com', 'KTHO-590.png', 0, 0, 0, 1, '2011-11-11', 2769, 0, '2017-01-01 00:00:00', 1), \
('The-Postal-Service', '', 'http://s7.viastreaming.net:7540', '', 'The-Postal-Service.png', 0, 0, 0, 1, '2011-11-11', 2770, 0, '2017-01-01 00:00:00', 1), \
('Stadt-Radio-Meran-875', '', 'http://server1.digital-webstream.de:37215', 'http://www.stadtradio-meran.it/', 'Stadt-Radio-Meran-875.png', 0, 0, 0, 1, '2011-11-11', 2779, 0, '2017-01-01 00:00:00', 1), \
('Radio-Jawhara-1025', '', 'http://www.jawharafm.net/live/jawharafm.m3u', 'http://www.jawharafm.net/', 'Radio-Jawhara-1025.png', 0, 0, 0, 1, '2011-11-11', 2788, 0, '2017-01-01 00:00:00', 1), \
('ATB', '', 'http://www.jawharafm.net/live/jawharafm.m3u', '', 'ATB.png', 0, 0, 0, 1, '2011-11-11', 2789, 0, '2017-01-01 00:00:00', 1), \
('Radio-Amore-i-migliori-anni-Catania-916', 'Radio Amore i migliori anni non &#232; una semplice radio ma un vero e proprio museo della musica anni &#39;60, &#39;70, &#39;80 curato da professionisti del settore e appassionati del genere. Il gran', 'http://onair11.xdevel.com:8032', 'http://www.grupporadioamore.it/', 'Radio-Amore-i-migliori-anni-Catania-916.png', 0, 0, 0, 1, '2011-11-11', 2796, 0, '2017-01-01 00:00:00', 1), \
('News-Talk-1540', '', 'http://onair11.xdevel.com:8032', 'http://www.kxel.com/', 'News-Talk-1540.png', 0, 0, 0, 1, '2011-11-11', 2797, 0, '2017-01-01 00:00:00', 1), \
('Lolli-Radio-Oldies', 'Il ritmo, lo stile, le emozioni della musica anni &#39;70 e &#39;80: solo quelle che hanno segnato un epoca, solo quelle belle! Tutto questo &#232; LOLLIRADIO OLDIES, la radio di &quot;vecchi&quot; su', 'http://www.lolliradio.net/streaming/oldies.asx', 'http://www.lolliradio.net/', 'Lolli-Radio-Oldies.png', 0, 0, 0, 1, '2011-11-11', 3359, 0, '2017-01-01 00:00:00', 1), \
('Cat-Country-961', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', 'http://www.ksly.com/', 'Cat-Country-961.png', 0, 0, 0, 1, '2011-11-11', 3367, 0, '2017-01-01 00:00:00', 1), \
('The-Coast-973', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', 'http://coastfm.com/', 'The-Coast-973.png', 0, 0, 0, 1, '2011-11-11', 3368, 0, '2017-01-01 00:00:00', 1), \
('Michael-Jackson', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', '', 'Michael-Jackson.png', 0, 0, 0, 1, '2011-11-11', 3369, 0, '2017-01-01 00:00:00', 1), \
('The-Early-Hours', '', 'http://asx.abacast.com/eldorado-ksly-64.pls', '', 'The-Early-Hours.png', 0, 0, 0, 1, '2011-11-11', 3370, 0, '2017-01-01 00:00:00', 1), \
('YLE-Radio-Suomi-940', '', 'http://yle.fi/livestream/radiosuomi.asx', 'http://www.yle.fi/radiosuomi/', 'YLE-Radio-Suomi-940.png', 0, 0, 0, 1, '2011-11-11', 3379, 0, '2017-01-01 00:00:00', 1), \
('Dig-Jazz', '', 'http://yle.fi/livestream/radiosuomi.asx', '', 'Dig-Jazz.png', 0, 0, 0, 1, '2011-11-11', 3380, 0, '2017-01-01 00:00:00', 1), \
('Maines-Coast-931', '', 'http://yle.fi/livestream/radiosuomi.asx', 'http://www.coast931.com/', 'Maines-Coast-931.png', 0, 0, 0, 1, '2011-11-11', 3381, 0, '2017-01-01 00:00:00', 1), \
('Wired-965', '', 'http://yle.fi/livestream/radiosuomi.asx', 'http://www.wired965.com/', 'Wired-965.png', 0, 0, 0, 1, '2011-11-11', 3382, 0, '2017-01-01 00:00:00', 1), \
('Frecuencia-Primera', '', 'http://67.212.179.138:8084', 'http://www.frecuenciaprimera.com/', 'Frecuencia-Primera.png', 0, 0, 0, 1, '2011-11-11', 3390, 0, '2017-01-01 00:00:00', 1), \
('Atlantic-Oldies-2NG', 'Great Oldies from the American and British Charts from the 50s to the 80s', 'http://www.2ngradio.com/2NG_BB_AAC.pls', 'http://www.2ngradio.com/', 'Atlantic-Oldies-2NG.png', 0, 0, 0, 1, '2011-11-11', 3391, 0, '2017-01-01 00:00:00', 1), \
('ESPN-HD3-981', 'Local programming from ESPN&#39;s Owned and Operated stations', 'http://www.2ngradio.com/2NG_BB_AAC.pls', 'http://www.espnradio.com/', 'ESPN-HD3-981.png', 0, 0, 0, 1, '2011-11-11', 3392, 0, '2017-01-01 00:00:00', 1), \
('radioeins-vom-rbb-958', '&quot;Nur f&#252;r Erwachsene&quot; ist radioeins weder Musiksender noch pure Information sondern ein lebendiger Mix aus beidem. Mit einer einzigartigen Musikvielfalt, handverlesen und gut gemischt so', 'http://www.radioeins.de/live.m3u', 'http://www.radioeins.de/', 'radioeins-vom-rbb-958.png', 0, 0, 0, 1, '2011-11-11', 3401, 0, '2017-01-01 00:00:00', 1), \
('The-Felice-Brothers', '', 'http://www.radioeins.de/live.m3u', '', 'The-Felice-Brothers.png', 0, 0, 0, 1, '2011-11-11', 3402, 0, '2017-01-01 00:00:00', 1), \
('Saturday-RaceDay', '', 'http://www.radioeins.de/live.m3u', '', 'Saturday-RaceDay.png', 0, 0, 0, 1, '2011-11-11', 3403, 0, '2017-01-01 00:00:00', 1), \
('Westsound-AM-1035', '', 'http://www.radioeins.de/live.m3u', 'http://www.westsound.co.uk/', 'Westsound-AM-1035.png', 0, 0, 0, 1, '2011-11-11', 3404, 0, '2017-01-01 00:00:00', 1), \
('The-Best-of-Mike-Gallagher', '', 'http://www.radioeins.de/live.m3u', '', 'The-Best-of-Mike-Gallagher.png', 0, 0, 0, 1, '2011-11-11', 3405, 0, '2017-01-01 00:00:00', 1), \
('94-Rock-941', '', 'http://www.radioeins.de/live.m3u', 'http://www.94rock.com/', '94-Rock-941.png', 0, 0, 0, 1, '2011-11-11', 3406, 0, '2017-01-01 00:00:00', 1), \
('Safri-Duo', '', 'http://www.radioeins.de/live.m3u', '', 'Safri-Duo.png', 0, 0, 0, 1, '2011-11-11', 3407, 0, '2017-01-01 00:00:00', 1), \
('WFMD-930', '', 'http://www.radioeins.de/live.m3u', 'http://www.wfmd.com/', 'WFMD-930.png', 0, 0, 0, 1, '2011-11-11', 3408, 0, '2017-01-01 00:00:00', 1), \
('Ghostface-Killah', '', 'http://www.radioeins.de/live.m3u', '', 'Ghostface-Killah.png', 0, 0, 0, 1, '2011-11-11', 3409, 0, '2017-01-01 00:00:00', 1), \
('The-Beat', '', 'http://www.radioeins.de/live.m3u', '', 'The-Beat.png', 0, 0, 0, 1, '2011-11-11', 3410, 0, '2017-01-01 00:00:00', 1), \
('Rick-James', '', 'http://www.radioeins.de/live.m3u', '', 'Rick-James.png', 0, 0, 0, 1, '2011-11-11', 3411, 0, '2017-01-01 00:00:00', 1), \
('Cataract', '', 'http://www.radioeins.de/live.m3u', '', 'Cataract.png', 0, 0, 0, 1, '2011-11-11', 3412, 0, '2017-01-01 00:00:00', 1), \
('Buzzoutroom-Radio', 'The Buzzoutroom is something that everyone should have in their house.', 'http://www.buzzoutroom.com/listen64.pls', 'http://www.buzzoutroom.com/', 'Buzzoutroom-Radio.png', 0, 0, 0, 1, '2011-11-11', 3419, 0, '2017-01-01 00:00:00', 1), \
('Mika', '', 'http://www.buzzoutroom.com/listen64.pls', '', 'Mika.png', 0, 0, 0, 1, '2011-11-11', 3420, 0, '2017-01-01 00:00:00', 1), \
('SKYFM-Love-Music', 'Easy listening and Romantic hits from the heart!!', 'http://listen.sky.fm/partner_mp3/lovemusic.pls', 'http://www.sky.fm/lovemusic', 'SKYFM-Love-Music.png', 0, 0, 0, 1, '2011-11-11', 3429, 0, '2017-01-01 00:00:00', 1), \
('Steely-Dan', '', 'http://kucr.org/64.m3u', '', 'Steely-Dan.png', 0, 0, 0, 1, '2011-11-11', 3438, 0, '2017-01-01 00:00:00', 1), \
('KPRT-1590', '', 'http://kucr.org/64.m3u', 'http://www.kprt.com/', 'KPRT-1590.png', 0, 0, 0, 1, '2011-11-11', 3439, 0, '2017-01-01 00:00:00', 1), \
('Super-country-105-1049', '', 'http://kucr.org/64.m3u', 'http://www.supercountry105.com/', 'Super-country-105-1049.png', 0, 0, 0, 1, '2011-11-11', 3440, 0, '2017-01-01 00:00:00', 1), \
('Within-Temptation', '', 'http://kucr.org/64.m3u', '', 'Within-Temptation.png', 0, 0, 0, 1, '2011-11-11', 3441, 0, '2017-01-01 00:00:00', 1), \
('Staind', '', 'http://kucr.org/64.m3u', '', 'Staind.png', 0, 0, 0, 1, '2011-11-11', 3442, 0, '2017-01-01 00:00:00', 1), \
('The-Southern-Football-Kickoff-Show', '', 'http://abc.net.au/res/streaming/audio/aac/local_darwin.pls', '', 'The-Southern-Football-Kickoff-Show.png', 0, 0, 0, 1, '2011-11-11', 3477, 0, '2017-01-01 00:00:00', 1), \
('The-BEAT-JAMZ-1011', '', 'http://abc.net.au/res/streaming/audio/aac/local_darwin.pls', 'http://www.101thebeat.com/', 'The-BEAT-JAMZ-1011.png', 0, 0, 0, 1, '2011-11-11', 3479, 0, '2017-01-01 00:00:00', 1), \
('WMEE-973', '', 'http://asx.abacast.com/federatedmedia-wmeefm-32.pls', 'http://www.wmee.com/', 'WMEE-973.png', 0, 0, 0, 1, '2011-11-11', 3488, 0, '2017-01-01 00:00:00', 1), \
('Money-Planning-Hour', '', 'http://asx.abacast.com/federatedmedia-wmeefm-32.pls', '', 'Money-Planning-Hour.png', 0, 0, 0, 1, '2011-11-11', 3489, 0, '2017-01-01 00:00:00', 1), \
('radio-1190', '', 'http://radio1190.colorado.edu:8000/high.mp3', 'http://www.radio1190.org/', 'radio-1190.png', 0, 0, 0, 1, '2011-11-11', 3497, 0, '2017-01-01 00:00:00', 1), \
('Neon-Productions-Radio', 'Great Indie Music In All Genres!  Hall of Fame Songwriters, Award Winning Singer/Songwriters!', 'http://www.serverroom.us/asx/listen268222.m3u', 'http://www.musictogousa.com', 'Neon-Productions-Radio.png', 0, 0, 0, 1, '2011-11-11', 3504, 0, '2017-01-01 00:00:00', 1), \
('USC-Trojans-at-Notre-Dame-Fighting-Irish-Oct-22-2011', '', 'http://www.miastomuzyki.pl/it/rmfsprzeboje.pls', '', 'USC-Trojans-at-Notre-Dame-Fighting-Irish-Oct-22-2011.png', 0, 0, 0, 1, '2011-11-11', 3512, 0, '2017-01-01 00:00:00', 1), \
('WUTC-881', '', 'http://www.publicbroadcasting.net/wutc/ppr/wutc.pls', 'http://www.wutc.org/', 'WUTC-881.png', 0, 0, 0, 1, '2011-11-11', 3514, 0, '2017-01-01 00:00:00', 1), \
('WMEQ-880', '', 'http://www.publicbroadcasting.net/wutc/ppr/wutc.pls', 'http://www.wmeq.com/', 'WMEQ-880.png', 0, 0, 0, 1, '2011-11-11', 3516, 0, '2017-01-01 00:00:00', 1), \
('Wave-105-1052', '', 'http://www.publicbroadcasting.net/wutc/ppr/wutc.pls', 'http://www.wave105.com/', 'Wave-105-1052.png', 0, 0, 0, 1, '2011-11-11', 3517, 0, '2017-01-01 00:00:00', 1), \
('WKIP-1450', '', 'http://www.publicbroadcasting.net/wutc/ppr/wutc.pls', 'http://www.1450wkip.com/', 'WKIP-1450.png', 0, 0, 0, 1, '2011-11-11', 3518, 0, '2017-01-01 00:00:00', 1), \
('Ringside-Report', '', 'http://www.publicbroadcasting.net/wutc/ppr/wutc.pls', '', 'Ringside-Report.png', 0, 0, 0, 1, '2011-11-11', 3519, 0, '2017-01-01 00:00:00', 1), \
('Dreamradio-Soft-Rock', 'Soft Rock from the 60s to now.', 'http://api.orpanc.net/listen/soft1.m3u', 'http://dreamradio.ca', 'Dreamradio-Soft-Rock.png', 0, 0, 0, 1, '2011-11-11', 3523, 0, '2017-01-01 00:00:00', 1), \
('WCHV-1260', '', 'http://api.orpanc.net/listen/soft1.m3u', 'http://www.wchv.com/', 'WCHV-1260.png', 0, 0, 0, 1, '2011-11-11', 3524, 0, '2017-01-01 00:00:00', 1), \
('Joe-Diffie', '', 'http://api.orpanc.net/listen/soft1.m3u', '', 'Joe-Diffie.png', 0, 0, 0, 1, '2011-11-11', 3525, 0, '2017-01-01 00:00:00', 1), \
('KDWN-720', '', 'http://api.orpanc.net/listen/soft1.m3u', 'http://www.kdwn.com/', 'KDWN-720.png', 0, 0, 0, 1, '2011-11-11', 3526, 0, '2017-01-01 00:00:00', 1), \
('Elvis-Only', '', 'http://api.orpanc.net/listen/soft1.m3u', '', 'Elvis-Only.png', 0, 0, 0, 1, '2011-11-11', 3527, 0, '2017-01-01 00:00:00', 1), \
('Retro-Online-Radio', '', 'http://www.retro-retro.ru/listen.pls', 'http://www.retro-retro.ru/', 'Retro-Online-Radio.png', 0, 0, 0, 1, '2011-11-11', 3536, 0, '2017-01-01 00:00:00', 1), \
('923-KRST-Country', '', 'http://www.retro-retro.ru/listen.pls', 'http://www.923krst.com/', '923-KRST-Country.png', 0, 0, 0, 1, '2011-11-11', 3537, 0, '2017-01-01 00:00:00', 1), \
('Radio-Rewind', '', 'http://listen.radionomy.com/radio-rewind', 'http://radio-rewind.playtheradio.com/', 'Radio-Rewind.png', 0, 0, 0, 1, '2011-11-11', 3540, 0, '2017-01-01 00:00:00', 1), \
('WZSK-1040', '', 'http://listen.radionomy.com/radio-rewind', '', 'WZSK-1040.png', 0, 0, 0, 1, '2011-11-11', 3541, 0, '2017-01-01 00:00:00', 1), \
('Kronehit-Radio-Krone-Hit-1034', '', 'http://onair.krone.at:80/kronehit.mp3', 'http://www.kronehit.at/', 'Kronehit-Radio-Krone-Hit-1034.png', 0, 0, 0, 1, '2011-11-11', 3542, 0, '2017-01-01 00:00:00', 1), \
('Jim-Blue-Gospel', '', 'http://onair.krone.at:80/kronehit.mp3', '', 'Jim-Blue-Gospel.png', 0, 0, 0, 1, '2011-11-11', 3543, 0, '2017-01-01 00:00:00', 1), \
('181FM-Party-181', '', 'http://listen.181fm.com/181-party_128k.mp3', 'http://www.181.fm/', '181FM-Party-181.png', 0, 0, 0, 1, '2011-11-11', 3559, 0, '2017-01-01 00:00:00', 1), \
('3Wi-959', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=3wiradio', 'http://www.3wiradio.com/', '3Wi-959.png', 0, 0, 0, 1, '2011-11-11', 3565, 0, '2017-01-01 00:00:00', 1), \
('KZQZ-1430', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=kzqz-am', 'http://kzqz1430am.com/', 'KZQZ-1430.png', 0, 0, 0, 1, '2011-11-11', 3567, 0, '2017-01-01 00:00:00', 1), \
('V-103-1033', 'V-103 is a Hip-Hop,', 'http://www.lolliradio.net/streaming/oldies.asx', 'http://www.v-103.com', 'V-103-1033.png', 0, 0, 0, 1, '2011-11-11', 3569, 0, '2017-01-01 00:00:00', 1), \
('Fast-Talk-(PRN)', '', 'http://www.lolliradio.net/streaming/oldies.asx', '', 'Fast-Talk-(PRN).png', 0, 0, 0, 1, '2011-11-11', 3570, 0, '2017-01-01 00:00:00', 1), \
('El-Gouna-Radio-1000', '', 'http://82.201.132.237:8000', 'http://www.romolo.com/', 'El-Gouna-Radio-1000.png', 0, 0, 0, 1, '2011-11-11', 3574, 0, '2017-01-01 00:00:00', 1), \
('James-Morrison', '', 'http://82.201.132.237:8000', '', 'James-Morrison.png', 0, 0, 0, 1, '2011-11-11', 3575, 0, '2017-01-01 00:00:00', 1), \
('Full-Throttle', '', 'http://82.201.132.237:8000', '', 'Full-Throttle.png', 0, 0, 0, 1, '2011-11-11', 3576, 0, '2017-01-01 00:00:00', 1), \
('WERS-889', '', 'http://www.wers.org/wers.pls', 'http://www.wers.org/', 'WERS-889.png', 0, 0, 0, 1, '2011-11-11', 3577, 0, '2017-01-01 00:00:00', 1), \
('Jesus-Jones', '', 'http://www.wers.org/wers.pls', '', 'Jesus-Jones.png', 0, 0, 0, 1, '2011-11-11', 3579, 0, '2017-01-01 00:00:00', 1), \
('Crocodiles', '', 'http://www.wers.org/wers.pls', '', 'Crocodiles.png', 0, 0, 0, 1, '2011-11-11', 3581, 0, '2017-01-01 00:00:00', 1), \
('181FM-Highway-181', '', 'http://listen.181fm.com/181-highway_128k.mp3', 'http://www.181.fm/', '181FM-Highway-181.png', 0, 0, 0, 1, '2011-11-11', 3583, 0, '2017-01-01 00:00:00', 1), \
('The-Human-League', '', 'http://www.181.fm/winamp.pls?station=181-highway&style=&description=Highway%20181', '', 'The-Human-League.png', 0, 0, 0, 1, '2011-11-11', 3585, 0, '2017-01-01 00:00:00', 1), \
('The-Dirt-Doctor-Radio-Show', '', 'http://www.181.fm/winamp.pls?station=181-highway&style=&description=Highway%20181', '', 'The-Dirt-Doctor-Radio-Show.png', 0, 0, 0, 1, '2011-11-11', 3587, 0, '2017-01-01 00:00:00', 1), \
('Urology--You', '', 'http://stream.lounge.fm/loungefm128.pls', '', 'Urology--You.png', 0, 0, 0, 1, '2011-11-11', 3588, 0, '2017-01-01 00:00:00', 1), \
('The-Barb-Mack-Polka-Show', '', 'http://www.181.fm/winamp.pls?station=181-highway&style=&description=Highway%20181', '', 'The-Barb-Mack-Polka-Show.png', 0, 0, 0, 1, '2011-11-11', 3589, 0, '2017-01-01 00:00:00', 1), \
('Bethany-Curve', '', 'http://www.radiofreemoscow.com/documents/listen_windowsmedia.asx', '', 'Bethany-Curve.png', 0, 0, 0, 1, '2011-11-11', 3590, 0, '2017-01-01 00:00:00', 1), \
('KMZU-1007', '', 'http://asx.abacast.com/kanza-kanza1-64.asx', 'http://www.kmzu.com/', 'KMZU-1007.png', 0, 0, 0, 1, '2011-11-11', 3592, 0, '2017-01-01 00:00:00', 1), \
('The-EAGLE-951', '', 'http://asx.abacast.com/kanza-kanza1-64.asx', 'http://www.951theeagle.com/', 'The-EAGLE-951.png', 0, 0, 0, 1, '2011-11-11', 3593, 0, '2017-01-01 00:00:00', 1), \
('Marca-Motor', '', 'http://www.181.fm/winamp.pls?station=181-90salt&style=&description=90%27s%20Alternative', '', 'Marca-Motor.png', 0, 0, 0, 1, '2011-11-11', 3594, 0, '2017-01-01 00:00:00', 1), \
('House-Bomb-FN', '', 'http://stream.laut.fm/housebomb-fn', 'http://www.laut.fm/housebomb-fn', 'House-Bomb-FN.png', 0, 0, 0, 1, '2011-11-11', 3597, 0, '2017-01-01 00:00:00', 1), \
('Gardening-with-Bob-Webster', '', 'http://listen.radionomy.com/french-station', '', 'Gardening-with-Bob-Webster.png', 0, 0, 0, 1, '2011-11-11', 3602, 0, '2017-01-01 00:00:00', 1), \
('Turkish-Ny-Radio', '', 'http://www.turkishny.com:8000/', 'http://www.turkishnyradio.com/', 'Turkish-Ny-Radio.png', 0, 0, 0, 1, '2011-11-11', 3616, 0, '2017-01-01 00:00:00', 1), \
('KRFP-925', 'Radio Free Moscow is a nonprofit,', 'http://www.radiofreemoscow.com/documents/listen_windowsmedia.asx', 'http://www.radiofreemoscow.com/', 'KRFP-925.png', 0, 0, 0, 1, '2011-11-11', 3617, 0, '2017-01-01 00:00:00', 1), \
('French-Station-Radio', '', 'http://listen.radionomy.com/french-station', 'http://frenchstation.weebly.com/', 'French-Station-Radio.png', 0, 0, 0, 1, '2011-11-11', 3625, 0, '2017-01-01 00:00:00', 1), \
('Paul-Desmond', '', 'http://listen.radionomy.com/french-station', '', 'Paul-Desmond.png', 0, 0, 0, 1, '2011-11-11', 3627, 0, '2017-01-01 00:00:00', 1), \
('The-Wolf-933', '', 'http://listen.radionomy.com/french-station', 'http://www.933ncd.com/', 'The-Wolf-933.png', 0, 0, 0, 1, '2011-11-11', 3628, 0, '2017-01-01 00:00:00', 1), \
('Giacomo-Gates', '', 'http://listen.radionomy.com/french-station', '', 'Giacomo-Gates.png', 0, 0, 0, 1, '2011-11-11', 3629, 0, '2017-01-01 00:00:00', 1), \
('Heart-Sussex-1035', 'Heart 102.4 &amp; 103.5 with Tom.', 'http://media-ice.musicradio.com/HeartSussexMP3', 'http://www.heartsussex.co.uk/', 'Heart-Sussex-1035.png', 0, 0, 0, 1, '2011-11-11', 3641, 0, '2017-01-01 00:00:00', 1), \
('181FM-90s-Alternative', '', 'http://listen.181fm.com/181-90salt_128k.mp3', 'http://www.181.fm/', '181FM-90s-Alternative.png', 0, 0, 0, 1, '2011-11-11', 3647, 0, '2017-01-01 00:00:00', 1), \
('WSRY-1550', '', 'http://www.181.fm/winamp.pls?station=181-chilled&style=mp3&description=Chilled%20Out', '', 'WSRY-1550.png', 0, 0, 0, 1, '2011-11-11', 3648, 0, '2017-01-01 00:00:00', 1);";



const char *radio_station_setupsql8="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('LoungeFM-958', 'Listen &amp; Relax', 'http://stream.lounge.fm/loungefm128.pls', 'http://www.lounge.fm/', 'LoungeFM-958.png', 0, 0, 0, 1, '2011-11-11', 3653, 0, '2017-01-01 00:00:00', 1), \
('Junior-Sisk--Ramblers-Choice', '', 'http://stream.lounge.fm/loungefm128.pls', '', 'Junior-Sisk--Ramblers-Choice.png', 0, 0, 0, 1, '2011-11-11', 3654, 0, '2017-01-01 00:00:00', 1), \
('985-KRZ', '', 'http://www.laradiodesebb.com/stream/classicandjazz.pls', 'http://www.985krz.com/', '985-KRZ.png', 0, 0, 0, 1, '2011-11-11', 3655, 0, '2017-01-01 00:00:00', 1), \
('Cat-Stevens', '', 'http://stream.lounge.fm/loungefm128.pls', '', 'Cat-Stevens.png', 0, 0, 0, 1, '2011-11-11', 3657, 0, '2017-01-01 00:00:00', 1), \
('The-Game-730-AM', '', 'http://stream.lounge.fm/loungefm128.pls', 'http://www.thegame730am.com/', 'The-Game-730-AM.png', 0, 0, 0, 1, '2011-11-11', 3659, 0, '2017-01-01 00:00:00', 1), \
('Local-Church-Programming', '', 'http://heartbeatz.fm:8008/', '', 'Local-Church-Programming.png', 0, 0, 0, 1, '2011-11-11', 3660, 0, '2017-01-01 00:00:00', 1), \
('The-Buzz-1005', '', 'http://www.181.fm/winamp.pls?station=181-chilled&style=mp3&description=Chilled%20Out', 'http://www.1005thebuzz.com', 'The-Buzz-1005.png', 0, 0, 0, 1, '2011-11-11', 3662, 0, '2017-01-01 00:00:00', 1), \
('181FM-Chilled-Out', '', 'http://listen.181fm.com/181-chilled_128k.mp3', 'http://www.181.fm/', '181FM-Chilled-Out.png', 0, 0, 0, 1, '2011-11-11', 3667, 0, '2017-01-01 00:00:00', 1), \
('Craig-Morgan', '', 'http://www.181.fm/winamp.pls?station=181-chilled&style=mp3&description=Chilled%20Out', '', 'Craig-Morgan.png', 0, 0, 0, 1, '2011-11-11', 3668, 0, '2017-01-01 00:00:00', 1), \
('White-Zombie', '', 'http://heartbeatz.fm:8008/', '', 'White-Zombie.png', 0, 0, 0, 1, '2011-11-11', 3669, 0, '2017-01-01 00:00:00', 1), \
('Kutski', '', 'http://www.181.fm/winamp.pls?station=181-chilled&style=mp3&description=Chilled%20Out', '', 'Kutski.png', 0, 0, 0, 1, '2011-11-11', 3671, 0, '2017-01-01 00:00:00', 1), \
('ESPN-1480', '', 'http://asx.abacast.com/federatedmedia-wrswam-32.pls', 'http://www.espnwarsaw.com/', 'ESPN-1480.png', 0, 0, 0, 1, '2011-11-11', 3672, 0, '2017-01-01 00:00:00', 1), \
('KGED-1680', '', 'http://www.thehmongradio.com/source/listlive/1680AM.m3u', 'http://www.thehmongradio.com/', 'KGED-1680.png', 0, 0, 0, 1, '2011-11-11', 3676, 0, '2017-01-01 00:00:00', 1), \
('Classic-and-Jazz', 'le meilleur de la musique classique et du jazz, sur une seule et m&#234;me radio.', 'http://www.laradiodesebb.com/stream/classicandjazz.pls', 'http://www.classicandjazz.net/', 'Classic-and-Jazz.png', 0, 0, 0, 1, '2011-11-11', 3677, 0, '2017-01-01 00:00:00', 1), \
('WLNI-1059', '', 'http://asx.abacast.com/centennial-wlni-48.asx', 'http://www.wlni.com/', 'WLNI-1059.png', 0, 0, 0, 1, '2011-11-11', 3678, 0, '2017-01-01 00:00:00', 1), \
('Sunday-Morning-Sports-Page', '', 'http://asx.abacast.com/centennial-wlni-48.asx', '', 'Sunday-Morning-Sports-Page.png', 0, 0, 0, 1, '2011-11-11', 3679, 0, '2017-01-01 00:00:00', 1), \
('The-Dodos', '', 'http://abc.net.au/res/streaming/audio/aac/itinerant_one.pls', '', 'The-Dodos.png', 0, 0, 0, 1, '2011-11-11', 3681, 0, '2017-01-01 00:00:00', 1), \
('Heartbeatzfm', 'Your number one slow music webradio station. We&#39;re known for 100% pure romance 24 hours a day seven days a week.', 'http://heartbeatz.fm:8008/', 'http://www.heartbeatz.fm/', 'Heartbeatzfm.png', 0, 0, 0, 1, '2011-11-11', 3682, 0, '2017-01-01 00:00:00', 1), \
('KWCK-FM-999', '', 'http://streamer.tcworks.net:8000/kwck', 'http://www.kwck999.com/', 'KWCK-FM-999.png', 0, 0, 0, 1, '2011-11-11', 3739, 0, '2017-01-01 00:00:00', 1), \
('93-JDX-933', '', 'http://streamer.tcworks.net:8000/kwck', 'http://www.theradionetwork.com/93jdx.html', '93-JDX-933.png', 0, 0, 0, 1, '2011-11-11', 3742, 0, '2017-01-01 00:00:00', 1), \
('KPUA-670', 'KPUA is Hilo&#39;s news, sports and information source.  featuring up to date Hawaii News and the best live sports coverage on the island including UH athletics, SF Giants, SF 49ers, Monday Night Foot', 'http://streamer.tcworks.net:8000/kwck', 'http://www.kpua.net/', 'KPUA-670.png', 0, 0, 0, 1, '2011-11-11', 3745, 0, '2017-01-01 00:00:00', 1), \
('World-FM-Tawa-882', 'World FM is a low power FM (LPFM) radio station based in Tawa, Wellington, New Zealand.<BR>Our aim is to play a mix of some of the best world music, Kiwi classics, and a selection of radio programming', 'http://www.worldfm.co.nz:8882/worldfmstereo.mp3', 'http://www.worldfm.co.nz/', 'World-FM-Tawa-882.png', 0, 0, 0, 1, '2011-11-11', 3756, 0, '2017-01-01 00:00:00', 1), \
('The-Crusaders', '', 'http://www.worldfm.co.nz:8882/worldfmstereo.mp3', '', 'The-Crusaders.png', 0, 0, 0, 1, '2011-11-11', 3757, 0, '2017-01-01 00:00:00', 1), \
('RPR1-Top40', 'Sie h&#246;ren am liebsten die allerneueste Musik? Kein Problem mit unserem neuen Webchannel. Hier h&#246;ren Sie nur Hits aus den aktuellen Charts.', 'http://217.151.151.90/stream5', 'http://www.rpr1.de/', 'RPR1-Top40.png', 0, 0, 0, 1, '2011-11-11', 3769, 0, '2017-01-01 00:00:00', 1), \
('Kataklysm', '', 'http://68.189.126.122:8000', '', 'Kataklysm.png', 0, 0, 0, 1, '2011-11-11', 3774, 0, '2017-01-01 00:00:00', 1), \
('Jay-Sekulow-Live', '', 'http://bbc.co.uk/radio/listen/live/rs.asx', '', 'Jay-Sekulow-Live.png', 0, 0, 0, 1, '2011-11-11', 3776, 0, '2017-01-01 00:00:00', 1), \
('Allgaeuhit-10655', 'Inhalte sind Weltnachrichten, Lokalnachrichten sowie Programmbeitr&#228;ge mit den Themenschwerpunkten Wirtschaft, Politik, Sport, Unterhaltung, sowie Kultur und Brauchtum.', 'http://www.allgaeuhit.de/allgaeuhit.m3u', 'http://www.allgaeuhit.de/', 'Allgaeuhit-10655.png', 0, 0, 0, 1, '2011-11-11', 3787, 0, '2017-01-01 00:00:00', 1), \
('Keys-2-Retirement', '', 'http://www.allgaeuhit.de/allgaeuhit.m3u', '', 'Keys-2-Retirement.png', 0, 0, 0, 1, '2011-11-11', 3788, 0, '2017-01-01 00:00:00', 1), \
('TFM-966', '', 'http://www.allgaeuhit.de/allgaeuhit.m3u', 'http://www.tfmradio.co.uk/', 'TFM-966.png', 0, 0, 0, 1, '2011-11-11', 3789, 0, '2017-01-01 00:00:00', 1), \
('2b-FM', '2bfm was the first heard on February 12, 2007. The station focuses on Flanders. 2bfm is owned by Frederick Geeroms.<BR><BR>CHR-pop format with a focus 2bfm itself to a &#39;young adult&#39;audience. T', 'http://listen.radionomy.com/2bfm', 'http://www.2bfm.be/', '2b-FM.png', 0, 0, 0, 1, '2011-11-11', 3800, 0, '2017-01-01 00:00:00', 1), \
('Miyako-Disaster-Relief-FM-774', '', 'http://www.simulradio.jp/asx/FmMiyako.asx', 'http://www.ustream.tv/channel/miyakofm774', 'Miyako-Disaster-Relief-FM-774.png', 0, 0, 0, 1, '2011-11-11', 3812, 0, '2017-01-01 00:00:00', 1), \
('181FM-Old-School-HipHopRnB', '', 'http://relay.181.fm:8068', 'http://www.181.fm/', '181FM-Old-School-HipHopRnB.png', 0, 0, 0, 1, '2011-11-11', 3813, 0, '2017-01-01 00:00:00', 1), \
('Eskimo-Joe', '', 'http://www.181.fm/asx.php?station=181-oldschool&style=mp3&description=Old School HipHop/RnB', '', 'Eskimo-Joe.png', 0, 0, 0, 1, '2011-11-11', 3814, 0, '2017-01-01 00:00:00', 1), \
('Really-Big-Show-Saturday', '', 'http://www.181.fm/asx.php?station=181-oldschool&style=mp3&description=Old School HipHop/RnB', '', 'Really-Big-Show-Saturday.png', 0, 0, 0, 1, '2011-11-11', 3815, 0, '2017-01-01 00:00:00', 1), \
('Dubai-92-920', '', 'http://www.181.fm/asx.php?station=181-oldschool&style=mp3&description=Old School HipHop/RnB', 'http://www.dubai92.com/', 'Dubai-92-920.png', 0, 0, 0, 1, '2011-11-11', 3816, 0, '2017-01-01 00:00:00', 1), \
('Country-Music-24', '', 'http://countrymusic24.powerstream.de:9000', 'http://www.countrymusic24.com/', 'Country-Music-24.png', 0, 0, 0, 1, '2011-11-11', 3832, 0, '2017-01-01 00:00:00', 1), \
('AM-790', '', 'http://countrymusic24.powerstream.de:9000', 'http://www.790business.com/', 'AM-790.png', 0, 0, 0, 1, '2011-11-11', 3834, 0, '2017-01-01 00:00:00', 1), \
('WURD-900', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wurd-am', 'http://900amwurd.com/', 'WURD-900.png', 0, 0, 0, 1, '2011-11-11', 3835, 0, '2017-01-01 00:00:00', 1), \
('K95-955', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wurd-am', '', 'K95-955.png', 0, 0, 0, 1, '2011-11-11', 3836, 0, '2017-01-01 00:00:00', 1), \
('Rugby-League', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wurd-am', '', 'Rugby-League.png', 0, 0, 0, 1, '2011-11-11', 3838, 0, '2017-01-01 00:00:00', 1), \
('The-Lake-1065', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wurd-am', 'http://www.wmvx.com/', 'The-Lake-1065.png', 0, 0, 0, 1, '2011-11-11', 3839, 0, '2017-01-01 00:00:00', 1), \
('Big-985', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wurd-am', 'http://www.big985.com/', 'Big-985.png', 0, 0, 0, 1, '2011-11-11', 3840, 0, '2017-01-01 00:00:00', 1), \
('Bum-Bum-Radio-894', '', 'http://radio.bumbumradio.rs:8010/', 'http://www.bumbumradio.rs/', 'Bum-Bum-Radio-894.png', 0, 0, 0, 1, '2011-11-11', 3866, 0, '2017-01-01 00:00:00', 1), \
('WCTR-1530', '', 'http://radio.bumbumradio.rs:8010/', 'http://www.wctr.com/', 'WCTR-1530.png', 0, 0, 0, 1, '2011-11-11', 3867, 0, '2017-01-01 00:00:00', 1), \
('Rush', '', 'http://radio.bumbumradio.rs:8010/', '', 'Rush.png', 0, 0, 0, 1, '2011-11-11', 3868, 0, '2017-01-01 00:00:00', 1), \
('WFDL-1170', '', 'http://www.fastbytes.com/am1170.asx', 'http://www.am1170radio.com/', 'WFDL-1170.png', 0, 0, 0, 1, '2011-11-11', 3871, 0, '2017-01-01 00:00:00', 1), \
('Golf-AZ', '', 'http://www.fastbytes.com/am1170.asx', '', 'Golf-AZ.png', 0, 0, 0, 1, '2011-11-11', 3872, 0, '2017-01-01 00:00:00', 1), \
('radioBERLIN-888-vom-rbb-888', 'radioBERLIN 88,8 spielt sie alle: Robby Williams, Herbert Gr&#246;nemeyer, Bee Gees, Die Prinzen, Smokie, ABBA, Rosenstolz, Madonna und Nena: Die gr&#246;&#223;ten Pop-hits - den ganzen Tag. Und radio', 'http://www.radioberlin.de/live.m3u', 'http://www.radioberlin.de/', 'radioBERLIN-888-vom-rbb-888.png', 0, 0, 0, 1, '2011-11-11', 3873, 0, '2017-01-01 00:00:00', 1), \
('Diana-Ross', '', 'http://www.ysinfo.net/webcast/KUBA1600.asx', '', 'Diana-Ross.png', 0, 0, 0, 1, '2011-11-11', 3876, 0, '2017-01-01 00:00:00', 1), \
('BEST80-Radio-Dance', '80&#39;s 100% DANCE, FUNK, DISCO and House', 'http://listen.radionomy.com/best80dance', 'http://www.best80radio.com', 'BEST80-Radio-Dance.png', 0, 0, 0, 1, '2011-11-11', 3900, 0, '2017-01-01 00:00:00', 1), \
('The-Chordettes', '', 'http://listen.radionomy.com/best80dance', '', 'The-Chordettes.png', 0, 0, 0, 1, '2011-11-11', 3901, 0, '2017-01-01 00:00:00', 1), \
('Race-Day-on-Fox', '', 'http://listen.radionomy.com/best80dance', '', 'Race-Day-on-Fox.png', 0, 0, 0, 1, '2011-11-11', 3902, 0, '2017-01-01 00:00:00', 1), \
('KLSU-911', '', 'http://130.39.238.143:8002/listen.pls', 'http://www.klsu.fm/', 'KLSU-911.png', 0, 0, 0, 1, '2011-11-11', 3926, 0, '2017-01-01 00:00:00', 1), \
('WKCT-930', '', 'http://130.39.238.143:8002/listen.pls', 'http://www.93wkct.com/', 'WKCT-930.png', 0, 0, 0, 1, '2011-11-11', 3927, 0, '2017-01-01 00:00:00', 1), \
('Sandra-Crofford-Show', '', 'http://130.39.238.143:8002/listen.pls', '', 'Sandra-Crofford-Show.png', 0, 0, 0, 1, '2011-11-11', 3928, 0, '2017-01-01 00:00:00', 1), \
('WNZK-690', '', 'http://wnzk.birach.com:9000', 'http://www.birach.com/wnzk.html', 'WNZK-690.png', 0, 0, 0, 1, '2011-11-11', 3939, 0, '2017-01-01 00:00:00', 1), \
('The-1Xtra-Showcase', '', 'http://wnzk.birach.com:9000', '', 'The-1Xtra-Showcase.png', 0, 0, 0, 1, '2011-11-11', 3940, 0, '2017-01-01 00:00:00', 1), \
('Milow', '', 'http://wnzk.birach.com:9000', '', 'Milow.png', 0, 0, 0, 1, '2011-11-11', 3941, 0, '2017-01-01 00:00:00', 1), \
('2bfm-Classix', '', 'http://listen.radionomy.com/2bfm-classix', 'http://www.2bfm.be/', '2bfm-Classix.png', 0, 0, 0, 1, '2011-11-11', 3945, 0, '2017-01-01 00:00:00', 1), \
('SomaFM-Indie-Pop-Rocks', 'Indie Pop Rocks! features the latest unsigned bands to the hottest new independent artists from around the globe, with a liberal sprinkling of classic and favorite indie tunes mixed in. From The Pixie', 'http://api.somafm.com/indiepop130.pls', 'http://www.somafm.com/', 'SomaFM-Indie-Pop-Rocks.png', 0, 0, 0, 1, '2011-11-11', 3958, 0, '2017-01-01 00:00:00', 1), \
('Paul-OGrady', '', 'http://api.somafm.com/indiepop130.pls', '', 'Paul-OGrady.png', 0, 0, 0, 1, '2011-11-11', 3960, 0, '2017-01-01 00:00:00', 1), \
('FluxFM-Berlin-1006', 'Wir inspirieren Menschen, denen Musik wichtig ist, wir k&#252;mmern uns um Popkultur.', 'http://www.fluxfm.de/stream-berlin/', 'http://www.fluxfm.de/', 'FluxFM-Berlin-1006.png', 0, 0, 0, 1, '2011-11-11', 3963, 0, '2017-01-01 00:00:00', 1), \
('GTFM-1079', 'The local Radio station for Pontypridd and neighbouring parts of the Rhondda, Cynon and Taff Valleys.', 'http://www.gtfm.co.uk/gtfmstream/listen.pls', 'http://www.gtfm.co.uk/', 'GTFM-1079.png', 0, 0, 0, 1, '2011-11-11', 3964, 0, '2017-01-01 00:00:00', 1), \
('KHHO-850', '', 'http://www.gtfm.co.uk/gtfmstream/listen.pls', 'http://www.foxsports850.com/', 'KHHO-850.png', 0, 0, 0, 1, '2011-11-11', 3966, 0, '2017-01-01 00:00:00', 1), \
('The-Team-1340', '', 'http://www.gtfm.co.uk/gtfmstream/listen.pls', 'http://www.theteam1340.com', 'The-Team-1340.png', 0, 0, 0, 1, '2011-11-11', 3967, 0, '2017-01-01 00:00:00', 1), \
('Frank-Sinatra', '', 'http://www.gtfm.co.uk/gtfmstream/listen.pls', '', 'Frank-Sinatra.png', 0, 0, 0, 1, '2011-11-11', 3968, 0, '2017-01-01 00:00:00', 1), \
('Centre-Street-Church', '', 'http://www.gtfm.co.uk/gtfmstream/listen.pls', '', 'Centre-Street-Church.png', 0, 0, 0, 1, '2011-11-11', 3969, 0, '2017-01-01 00:00:00', 1), \
('Kim-Wilde', '', 'http://www.gtfm.co.uk/gtfmstream/listen.pls', '', 'Kim-Wilde.png', 0, 0, 0, 1, '2011-11-11', 3970, 0, '2017-01-01 00:00:00', 1), \
('Whisperings-Solo-Piano-Radio', 'Music to Quiet Your World', 'http://vistaweb.streamguys.com/pianosolo64k.asx', 'http://www.solopianoradio.com/', 'Whisperings-Solo-Piano-Radio.png', 0, 0, 0, 1, '2011-11-11', 3973, 0, '2017-01-01 00:00:00', 1), \
('K95-FM-955', '', 'http://vistaweb.streamguys.com/pianosolo64k.asx', 'http://www.k955fm.com/', 'K95-FM-955.png', 0, 0, 0, 1, '2011-11-11', 3974, 0, '2017-01-01 00:00:00', 1), \
('WTRX-Programming', '', 'http://vistaweb.streamguys.com/pianosolo64k.asx', '', 'WTRX-Programming.png', 0, 0, 0, 1, '2011-11-11', 3975, 0, '2017-01-01 00:00:00', 1), \
('Sport-und-Musik', '', 'http://vistaweb.streamguys.com/pianosolo64k.asx', '', 'Sport-und-Musik.png', 0, 0, 0, 1, '2011-11-11', 3976, 0, '2017-01-01 00:00:00', 1), \
('Lacy-J-Dalton', '', 'http://vistaweb.streamguys.com/pianosolo64k.asx', '', 'Lacy-J-Dalton.png', 0, 0, 0, 1, '2011-11-11', 3977, 0, '2017-01-01 00:00:00', 1), \
('Futu-Radio-Dance', '', 'http://www.futuradios.com/ecoute/dance/320/thd-winamp.pls', 'http://www.futuradios.com/', 'Futu-Radio-Dance.png', 0, 0, 0, 1, '2011-11-11', 3978, 0, '2017-01-01 00:00:00', 1), \
('The-Buzz-1430', '', 'http://www.futuradios.com/ecoute/dance/320/thd-winamp.pls', 'http://www.1430thebuzz.com/', 'The-Buzz-1430.png', 0, 0, 0, 1, '2011-11-11', 3979, 0, '2017-01-01 00:00:00', 1), \
('Chris-Connor', '', 'http://stm1.rthk.hk/radio5', '', 'Chris-Connor.png', 0, 0, 0, 1, '2011-11-11', 3985, 0, '2017-01-01 00:00:00', 1), \
('ESPN-1013', 'ESPN affiliate station serving the Burlington, VT and Plattsburgh areas.  Flagship station for the University of Vermont Men&#39;s basketball team and the Vermont Lake Monsters minor league baseball t', 'http://crystalout.surfernetwork.com:8001/WCPV_MP3', 'http://champrocks.com/', 'ESPN-1013.png', 0, 0, 0, 1, '2011-11-11', 3990, 0, '2017-01-01 00:00:00', 1), \
('WMAG-995', '', 'http://crystalout.surfernetwork.com:8001/WCPV_MP3', 'http://www.wmagradio.com/', 'WMAG-995.png', 0, 0, 0, 1, '2011-11-11', 3991, 0, '2017-01-01 00:00:00', 1), \
('No-Secrets', '', 'http://crystalout.surfernetwork.com:8001/WCPV_MP3', '', 'No-Secrets.png', 0, 0, 0, 1, '2011-11-11', 3992, 0, '2017-01-01 00:00:00', 1), \
('Limfjord-Plus-947', 'Varieret mix af god musik krydret med lokale, nationale og internationale nyheder.', 'http://www.limfjordnetradio.dk/limfjordplus.m3u', 'http://www.limfjordplus.dk/', 'Limfjord-Plus-947.png', 0, 0, 0, 1, '2011-11-11', 4014, 0, '2017-01-01 00:00:00', 1), \
('Jazz-in-the-Afternoon', '', 'http://www.limfjordnetradio.dk/limfjordplus.m3u', '', 'Jazz-in-the-Afternoon.png', 0, 0, 0, 1, '2011-11-11', 4015, 0, '2017-01-01 00:00:00', 1), \
('Q1063', '', 'http://www.limfjordnetradio.dk/limfjordplus.m3u', 'http://www.q106.com/', 'Q1063.png', 0, 0, 0, 1, '2011-11-11', 4016, 0, '2017-01-01 00:00:00', 1), \
('WKHG-1049', '', 'http://crystalout.surfernetwork.com:8001/WKHG_MP3', 'http://www.k105.com/', 'WKHG-1049.png', 0, 0, 0, 1, '2011-11-11', 4040, 0, '2017-01-01 00:00:00', 1), \
('Vox-Angeli', '', 'http://crystalout.surfernetwork.com:8001/WKHG_MP3', '', 'Vox-Angeli.png', 0, 0, 0, 1, '2011-11-11', 4041, 0, '2017-01-01 00:00:00', 1), \
('WUSF-897', '', 'http://mp3.wusf.usf.edu:8000/listen.pls', 'http://www.wusf.usf.edu/wusf-fm/', 'WUSF-897.png', 0, 0, 0, 1, '2011-11-11', 4048, 0, '2017-01-01 00:00:00', 1), \
('Joe-Satriani', '', 'http://mp3.wusf.usf.edu:8000/listen.pls', '', 'Joe-Satriani.png', 0, 0, 0, 1, '2011-11-11', 4049, 0, '2017-01-01 00:00:00', 1), \
('Rewind-Columbus-1035', '', 'http://mp3.wusf.usf.edu:8000/listen.pls', 'http://www.rewindcolumbus.com', 'Rewind-Columbus-1035.png', 0, 0, 0, 1, '2011-11-11', 4050, 0, '2017-01-01 00:00:00', 1), \
('WTQR-1041', '', 'http://mp3.wusf.usf.edu:8000/listen.pls', 'http://www.wtqr.com/', 'WTQR-1041.png', 0, 0, 0, 1, '2011-11-11', 4051, 0, '2017-01-01 00:00:00', 1), \
('ZRock-993', '', 'http://mp3.wusf.usf.edu:8000/listen.pls', 'http://www.zrock993.com/', 'ZRock-993.png', 0, 0, 0, 1, '2011-11-11', 4052, 0, '2017-01-01 00:00:00', 1), \
('Capital-South--West-Yorkshire-1051', 'The brand-new Capital Network from London, Scotland, the North East, Yorkshire, Manchester', 'http://media-ice.musicradio.com/CapitalYorkshireSouthWestMP3', 'http://www.capitalfm.com/yorkshiresouthwest/', 'Capital-South--West-Yorkshire-1051.png', 0, 0, 0, 1, '2011-11-11', 4064, 0, '2017-01-01 00:00:00', 1), \
('Sonrise', '', 'http://media-ice.musicradio.com/CapitalYorkshireSouthWestMP3', '', 'Sonrise.png', 0, 0, 0, 1, '2011-11-11', 4065, 0, '2017-01-01 00:00:00', 1), \
('PMD-Radio', 'We play the best 80&#39;s and 90&#39;s hits The best movie tunes and the best Disney music from the movies and the parks around the world.<BR><BR>Want your favorite song on PMD-Radio than send us an e', 'http://listen.radionomy.com/pmd-radio', 'http://www.radionomy.com/pmd-radio', 'PMD-Radio.png', 0, 0, 0, 1, '2011-11-11', 4074, 0, '2017-01-01 00:00:00', 1), \
('LandesWelle-Thueringen-1042', 'LandesWelle - Th&#252;ringens Rock und Pop Radio. Die meiste Abwechslung mit den besten Songs aller Zeiten. LandesWelle ist in Th&#252;ringen zuhause. Wir senden ein frisches Radioprogramm von Th&#252', 'http://stream.hoerradar.de/lwtmp3', 'http://www.landeswelle.de/', 'LandesWelle-Thueringen-1042.png', 0, 0, 0, 1, '2011-11-11', 4075, 0, '2017-01-01 00:00:00', 1), \
('Sponge', '', 'http://stream.hoerradar.de/lwtmp3', '', 'Sponge.png', 0, 0, 0, 1, '2011-11-11', 4076, 0, '2017-01-01 00:00:00', 1), \
('News-Talk-930', '', 'http://stream.hoerradar.de/lwtmp3', 'http://www.wlssradio.com/', 'News-Talk-930.png', 0, 0, 0, 1, '2011-11-11', 4077, 0, '2017-01-01 00:00:00', 1), \
('KKNW-1150', '', 'http://den-a.plr.liquidcompass.net/pls/KKNWAMMP3.pls', 'http://www.1150kknw.com/', 'KKNW-1150.png', 0, 0, 0, 1, '2011-11-11', 4095, 0, '2017-01-01 00:00:00', 1), \
('Dave-Mason', '', 'http://den-a.plr.liquidcompass.net/pls/KKNWAMMP3.pls', '', 'Dave-Mason.png', 0, 0, 0, 1, '2011-11-11', 4096, 0, '2017-01-01 00:00:00', 1), \
('Bob-Frantz', '', 'http://den-a.plr.liquidcompass.net/pls/KKNWAMMP3.pls', '', 'Bob-Frantz.png', 0, 0, 0, 1, '2011-11-11', 4097, 0, '2017-01-01 00:00:00', 1), \
('Two-Door-Cinema-Club', '', 'http://den-a.plr.liquidcompass.net/pls/KKNWAMMP3.pls', '', 'Two-Door-Cinema-Club.png', 0, 0, 0, 1, '2011-11-11', 4098, 0, '2017-01-01 00:00:00', 1), \
('psyradio*fm', '[Bitrate: 128]', 'http://81.88.36.42:8010', '', '', 0, 128, 0, 1, '2011-08-11', 4105, 8, NULL, 1), \
('Reggae Radio RastaMusic.com', '[Bitrate: 128] ', 'http://66.225.205.4:80', '', '', 48, 128, 0, 1, '2011-08-11', 4125, 7, NULL, 1), \
('AnimeRadio.SU', '[Bitrate: 128]  ', 'http://78.46.91.38:8000', '', '', 0, 128, 0, 1, '2011-08-11', 4168, 8, NULL, 1), \
('Public Domain Jazz', '[Bitrate: 128] ', 'http://82.197.165.138:80', '', '', 0, 128, 0, 1, '2011-08-11', 4180, 24, NULL, 1), \
('COUNTRY 108', '[Bitrate: 128]   ', 'http://80.237.210.44:80', '', '', 6, 128, 0, 1, '2011-08-11', 4185, 8, NULL, 1), \
('Shanson 101.9 Kiev', '[Bitrate: 128] ', 'http://217.20.164.163:8002', '', '', 0, 128, 1, 1, '2011-08-11', 4186, 49, '2011-10-14 16:11:15', 1), \
('STAR FM Bln', '[Bitrate: 128] ', 'http://87.230.53.43:8000', '', '', 1, 128, 0, 1, '2011-08-11', 4187, 12, NULL, 1), \
('DeepFM', '(DeepFM) [Bitrate: 128] ', 'http://195.66.134.40:80', '', '', 0, 128, 2, 1, '2011-08-11', 4192, 24, '2011-10-14 16:04:45', 1), \
('memoryradio', '[Bitrate: 128]  ', 'http://188.165.247.175:4000', '', '', 0, 128, 0, 1, '2011-08-11', 4195, 4, NULL, 1), \
('Kalasam.com', '[Bitrate: 128]  ', 'http://173.192.207.51:8084', '', '', 0, 128, 0, 1, '2011-08-11', 4222, 7, NULL, 1), \
('NAXI RADIO 96', '9MHz - Beograd - www.naxi.rs ', 'http://193.243.169.34:9150', '', '', 0, 128, 1, 1, '2011-08-11', 4247, 45, '2011-10-14 16:11:02', 1), \
('WKSU Classical', 'WKSU Classical: Ludwig van Beethoven - Piano Sonata No. 31  [Bitrate: 128] [Max Listeners: 2000]', 'http://66.225.205.8:8030', '', '', 8, 128, 0, 1, '2011-08-11', 4275, 7, NULL, 1), \
('Radio Stad', '[Willie And The Hand Jive]  [Bitrate: 128] ', 'http://81.173.3.250:80', '', '', 0, 128, 0, 1, '2011-08-11', 4279, 23, NULL, 1), \
('SLOW JAMZ For Lovers', '[Bitrate: 128] ', 'http://85.25.16.7:8028', '', '', 0, 128, 0, 1, '2011-08-11', 4290, 8, NULL, 1), \
('Scansat', 'Scansat Trondheim  [Bitrate: 128]  ', 'http://212.62.227.4:2049', '', '', 0, 128, 0, 1, '2011-08-11', 4291, 21, NULL, 1), \
('Antena Krusevac Studio 1', '  [Bitrate: 128]  ', 'http://94.23.38.225:4560', '', '', 51, 128, 0, 1, '2011-08-11', 4305, 4, NULL, 1), \
('HAPPYDAY', '[Bitrate: 128]  ', 'http://222.122.131.69:8000', '', '', 0, 128, 1, 1, '2011-08-11', 4317, 40, NULL, 1), \
('Radio Beograd 202', '[Bitrate: 128] [Max Listeners: 2000]', 'http://195.252.107.194:8006', '', '', 0, 128, 0, 1, '2011-08-11', 4349, 3, NULL, 1), \
('fullspectrumradio.com', 'Drum and Bass', 'http://67.230.191.211:8000', '', '', 53, 320, 0, 1, '2011-08-16', 4539, 7, NULL, 1), \
('Schlagerhoelle', '[Bitrate: 128]', 'http://84.19.184.27:30842', '', '', 0, 128, 0, 1, '2011-08-11', 4596, 8, NULL, 1), \
('Radio Paloma - 100% Deutscher Schlager!', '[Bitrate: 128]', 'http://80.237.184.23:80', '', '', 45, 128, 0, 1, '2011-08-11', 4619, 8, NULL, 1), \
('Venice Classic', '[Bitrate: 128]  ', 'http://174.36.206.197:8000', '', '', 8, 128, 0, 1, '2011-08-11', 4624, 58, NULL, 1), \
('HIT104', '[Bitrate: 128]', 'http://80.237.210.57:80', '', '', 40, 128, 0, 1, '2011-08-11', 4629, 8, NULL, 1), \
('La X Estereo', '[Bitrate: 128]', 'http://80.237.211.85:80', '', '', 0, 128, 0, 1, '2011-08-11', 4630, 8, NULL, 1), \
('RMNradio', '[Bitrate: 128]', 'http://80.237.201.92:8010', '', '', 0, 128, 0, 1, '2011-08-11', 4632, 8, NULL, 1), \
('RadioMv.com', '[Bitrate: 128]', 'http://65.19.173.132:2204', '', '', 0, 128, 0, 1, '2011-08-11', 4634, 7, NULL, 1), \
('Beatles Radio.com', 'Solos', 'http://64.40.99.2:8088', '', '', 21, 128, 0, 1, '2011-08-11', 4637, 27, NULL, 1), \
('RADIODISCOPOLO.FM', ' radio ZET', 'http://94.23.36.107:443', '', '', 0, 128, 0, 1, '2011-08-11', 4652, 4, NULL, 1), \
('LOLLIRADIO ITALIA ', '[Bitrate: 128]', 'http://94.23.67.172:8010', '', '', 0, 128, 0, 1, '2011-08-11', 4669, 26, NULL, 1), \
('Sharmanka 104.0 Kiev', '[Bitrate: 128]', 'http://217.20.164.163:8006', '', '', 0, 128, 1, 1, '2011-08-11', 4673, 49, '2011-10-14 16:11:19', 1), \
('JACK FM - ALL HIT RADIO', '[Bitrate: 128]', 'http://80.237.210.91:80', '', '', 0, 128, 0, 1, '2011-08-11', 4687, 8, NULL, 1), \
('Folk Alley', 'FolkAlley.com  [Bitrate: 128]', 'http://66.225.205.8:80', '', '', 51, 128, 0, 1, '2011-08-11', 4698, 7, NULL, 1), \
('Raggakings the 24-7', 'Bersenbrueck ReggaeJam 2011 - Ranking Joe  [Bitrate: 128] ', 'http://64.202.98.51:7970', '', '', 0, 128, 0, 1, '2011-08-11', 4702, 7, NULL, 1), \
('Springsteenradio.com', '[Bitrate: 128]   ', 'http://67.212.166.178:9000', '', '', 21, 128, 0, 1, '2011-08-11', 4728, 7, NULL, 1), \
('psyradio*fm', 'chillchannel [Bitrate: 128] [Max Listeners: 410]', 'http://81.88.36.42:8020', '', '', 0, 128, 0, 1, '2011-08-11', 4757, 8, NULL, 1), \
('Radio Beograd 1', '[Bitrate: 128] [Max Listeners: 2000]', 'http://195.252.107.194:8002', '', '', 0, 128, 0, 1, '2011-08-11', 4763, 3, NULL, 1), \
('Raven-Symon', '', 'http://rs1.radiostreamer.com:8070/', '', 'Raven-Symon.png', 0, 0, 0, 1, '2011-11-11', 4807, 0, '2017-01-01 00:00:00', 1), \
('Cheverus-vs-Biddeford-Oct-8-2011', '', 'http://rs1.radiostreamer.com:8070/', '', 'Cheverus-vs-Biddeford-Oct-8-2011.png', 0, 0, 0, 1, '2011-11-11', 4808, 0, '2017-01-01 00:00:00', 1), \
('Oracle-Broadcasting', 'The Oracle Broadcasting Radio Network strives to be the, &quot;The Home Of Cutting Edge Talk Radio.&quot; Oracle Broadcasting streams the best uncensored talk radio, geared to freedom and liberty mind', 'http://oraclebroadcasting.com/64k.pls', 'http://www.oraclebroadcasting.com/', 'Oracle-Broadcasting.png', 0, 0, 0, 1, '2011-11-11', 4826, 0, '2017-01-01 00:00:00', 1), \
('NOVA-fm', 'Danmarks st&#248;rste kommercielle radiostation.', 'http://stream.novafm.dk/nova128', 'http://novafm.dk/', 'NOVA-fm-914.png', 0, 0, 0, 1, '2011-11-11', 4827, 0, '2017-01-01 00:00:00', 1), \
('Rocky-FM', 'Hier spielen exklusiv die gr&#246;&#223;ten Rocksongs aller Zeiten; von Alanis Morissette, Bryan Adams, Bon Jovi, Tom Petty bis zu Fleetwood Mac sowie die neuesten und besten Rocksongs von Maroon 5, D', 'http://www.rockyfm.de/listen.asx', 'http://www.rockyfm.de/', 'Rocky-FM.png', 0, 0, 0, 1, '2011-11-11', 4861, 0, '2017-01-01 00:00:00', 1), \
('Old-School-Freight-Train', '', 'http://www.rockyfm.de/listen.asx', '', 'Old-School-Freight-Train.png', 0, 0, 0, 1, '2011-11-11', 4862, 0, '2017-01-01 00:00:00', 1), \
('WWIS-FM-997', 'Tune to FM 99.7 for all of your classic country favorites. AND don&#39;t miss coverage of your favorite local sports teams plus Badger Football and Basketball, NFL Football and Brewers Baseball.', 'http://www.shinyrock.com/wwisradio.asx', 'http://www.wwisradio.com/', 'WWIS-FM-997.png', 0, 0, 0, 1, '2011-11-11', 4867, 0, '2017-01-01 00:00:00', 1), \
('Progressive-Radio-Network', 'Progressive Radio Network is the thinking person', 'http://216.55.165.146:8000', 'http://www.progressiveradionetwork.com/', 'Progressive-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 4868, 0, '2017-01-01 00:00:00', 1), \
('Kass-FM-891', '', 'http://media.kassfm.co.ke:8006/live', 'http://www.kassfm.co.ke/', 'Kass-FM-891.png', 0, 0, 0, 1, '2011-11-11', 4894, 0, '2017-01-01 00:00:00', 1), \
('Ambiance-Reggae', 'Une webradio &#224; la programmation musicale 100% reggae, Dub, Pop-Reggae, Ragga et Roots Reggae, en continu et en direct de Paris.', 'http://listen.radionomy.com/ambiance-reggae', 'http://ambiance-reggae.playtheradio.com/', 'Ambiance-Reggae.png', 0, 0, 1, 1, '2011-11-11', 4901, 0, '2011-10-14 16:00:16', 1), \
('RTL-2-Guadeloupe-1062', '', 'http://radio-shoutcast.cyber-streaming.com:8090/', 'http://www.rtl2antilles.fr/', 'RTL-2-Guadeloupe-1062.png', 0, 0, 0, 1, '2011-11-11', 4920, 0, '2017-01-01 00:00:00', 1), \
('Fun-Radio-943', '', 'http://stream.funradio.sk:8000/fun128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-943.png', 0, 0, 0, 1, '2011-11-11', 4942, 0, '2017-01-01 00:00:00', 1), \
('WAEB-790', '', 'http://stream.funradio.sk:8000/fun128.mp3', 'http://www.waeb.com/', 'WAEB-790.png', 0, 0, 0, 1, '2011-11-11', 4944, 0, '2017-01-01 00:00:00', 1), \
('KTZN-950', '', 'http://stream.funradio.sk:8000/fun128.mp3', 'http://www.550thezone.com/', 'KTZN-950.png', 0, 0, 0, 1, '2011-11-11', 4945, 0, '2017-01-01 00:00:00', 1), \
('Kim-Waters', '', 'http://stream.funradio.sk:8000/fun128.mp3', '', 'Kim-Waters.png', 0, 0, 0, 1, '2011-11-11', 4946, 0, '2017-01-01 00:00:00', 1), \
('Kass-FM-891', '', 'http://media.kassfm.co.ke:8006/live', 'http://www.kassfm.co.ke/', 'Kass-FM-891.png', 0, 0, 0, 1, '2011-11-11', 5019, 0, '2017-01-01 00:00:00', 1), \
('Ambiance-Reggae', 'Une webradio &#224; la programmation musicale 100% reggae, Dub, Pop-Reggae, Ragga et Roots Reggae, en continu et en direct de Paris.', 'http://listen.radionomy.com/ambiance-reggae', 'http://ambiance-reggae.playtheradio.com/', 'Ambiance-Reggae.png', 0, 0, 1, 1, '2011-11-11', 5026, 0, '2011-10-14 16:00:11', 1), \
('RTL-2-Guadeloupe-1062', '', 'http://radio-shoutcast.cyber-streaming.com:8090/', 'http://www.rtl2antilles.fr/', 'RTL-2-Guadeloupe-1062.png', 0, 0, 0, 1, '2011-11-11', 5045, 0, '2017-01-01 00:00:00', 1), \
('Conversations-at-the-Well', '', 'http://radio-shoutcast.cyber-streaming.com:8090/', '', 'Conversations-at-the-Well.png', 0, 0, 0, 1, '2011-11-11', 5046, 0, '2017-01-01 00:00:00', 1), \
('Minnie-Riperton', '', 'http://radio-shoutcast.cyber-streaming.com:8090/', '', 'Minnie-Riperton.png', 0, 0, 0, 1, '2011-11-11', 5047, 0, '2017-01-01 00:00:00', 1), \
('Fun-Radio-943', '', 'http://stream.funradio.sk:8000/fun128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-943.png', 0, 0, 0, 1, '2011-11-11', 5067, 0, '2017-01-01 00:00:00', 1), \
('FUV-Music-907', '', 'http://www.wfuv.org/sites/all/files/streams/fuv247_64k.pls', 'http://www.wfuv.org/programs/fuvmusic.html', 'FUV-Music-907.png', 0, 0, 0, 1, '2011-11-11', 5106, 0, '2017-01-01 00:00:00', 1), \
('Todays-word-from-the-Church-of-the-Advent', '', 'http://www.wfuv.org/sites/all/files/streams/fuv247_64k.pls', '', 'Todays-word-from-the-Church-of-the-Advent.png', 0, 0, 0, 1, '2011-11-11', 5107, 0, '2017-01-01 00:00:00', 1), \
('Fontella-Bass', '', 'http://www.wfuv.org/sites/all/files/streams/fuv247_64k.pls', '', 'Fontella-Bass.png', 0, 0, 0, 1, '2011-11-11', 5108, 0, '2017-01-01 00:00:00', 1), \
('Steel-Breeze', '', 'http://www.wfuv.org/sites/all/files/streams/fuv247_64k.pls', '', 'Steel-Breeze.png', 0, 0, 0, 1, '2011-11-11', 5109, 0, '2017-01-01 00:00:00', 1), \
('Jennifer-Lopez', '', 'http://www.wfuv.org/sites/all/files/streams/fuv247_64k.pls', '', 'Jennifer-Lopez.png', 0, 0, 0, 1, '2011-11-11', 5110, 0, '2017-01-01 00:00:00', 1), \
('NOVA-fm-914', 'Danmarks st&#248;rste kommercielle radiostation. Danmarks eneste landsd&#230;kkende kommercielle radiostation.', 'http://stream.novafm.dk/nova128', 'http://novafm.dk/', 'NOVA-fm-914.png', 0, 0, 0, 1, '2011-11-11', 5150, 0, '2017-01-01 00:00:00', 1);";

const char *radio_station_setupsql13="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('181FM-Kickin-Country', '', 'http://listen.181fm.com/181-kickincountry_128k.mp3', 'http://www.181.fm/', '181FM-Kickin-Country.png', 0, 0, 0, 1, '2011-11-11', 5295, 0, '2017-01-01 00:00:00', 1), \
('Cheche-80S--90S-Rock-Hits', 'Esta es una emisora especializada en clasicos, especialmente de las decadas 80&#180;s y 90&#180;s. La programacion abarca los diversos subgeneros del rock. Escucharas las bandas, grupos y artistas con', 'http://listen.radionomy.com/cheche-international-radio', 'http://www.elcheche.net', 'Cheche-80S--90S-Rock-Hits.png', 0, 0, 0, 1, '2011-11-11', 5324, 0, '2017-01-01 00:00:00', 1), \
('Sports-Talk', '', 'http://listen.radionomy.com/cheche-international-radio', '', 'Sports-Talk.png', 0, 0, 0, 1, '2011-11-11', 5326, 0, '2017-01-01 00:00:00', 1), \
('Radio-Listin-997', '', 'http://mekradio.mekstream.com/radiolistin', 'http://www.radiolistin.com/', 'Radio-Listin-997.png', 0, 0, 0, 1, '2011-11-11', 5338, 0, '2017-01-01 00:00:00', 1), \
('Black-Label-Society', '', 'http://azulweb.streamguys.com/spokanechiefs.asx', '', 'Black-Label-Society.png', 0, 0, 0, 1, '2011-11-11', 5354, 0, '2017-01-01 00:00:00', 1), \
('Gary-Moore', '', 'http://azulweb.streamguys.com/spokanechiefs.asx', '', 'Gary-Moore.png', 0, 0, 0, 1, '2011-11-11', 5355, 0, '2017-01-01 00:00:00', 1), \
('WKOZ-1340', '', 'http://azulweb.streamguys.com/spokanechiefs.asx', 'http://www.breezynews.com/banners/wkoz.htm', 'WKOZ-1340.png', 0, 0, 0, 1, '2011-11-11', 5356, 0, '2017-01-01 00:00:00', 1), \
('The-Devil-And-Me', '', 'http://azulweb.streamguys.com/spokanechiefs.asx', '', 'The-Devil-And-Me.png', 0, 0, 0, 1, '2011-11-11', 5357, 0, '2017-01-01 00:00:00', 1), \
('Jazz-Loft-Radio', 'Jazz, Lounge, Groove, Chill, Soul, Weltmusik, Nu Jazz und klassische T&#246;ne f&#252;r relaxte Tage und anregende N&#228;chte ...', 'http://stream.laut.fm/jazzloft', 'http://www.laut.fm/jazzloft', 'Jazz-Loft-Radio.png', 0, 0, 0, 1, '2011-11-11', 5358, 0, '2017-01-01 00:00:00', 1), \
('Diamond-Gems', '', 'http://stream.laut.fm/jazzloft', '', 'Diamond-Gems.png', 0, 0, 0, 1, '2011-11-11', 5359, 0, '2017-01-01 00:00:00', 1), \
('KBLU-560', '', 'http://asx.abacast.com/eldorado-kblu-64.pls', 'http://www.kbluam.com/', 'KBLU-560.png', 0, 0, 0, 1, '2011-11-11', 5360, 0, '2017-01-01 00:00:00', 1), \
('Inforadio-vom-rbb-931', 'Inforadio bietet 24 Stunden lang Nachrichten mit Hintergrund: Alle 20 Minuten News, Wetter und Verkehrsservice, anschlie&#223;end Berichte, Interviews und Repor-tagen zu den Topthemen des Tages. Rund ', 'http://www.inforadio.de/live.m3u', 'http://www.inforadio.de/', 'Inforadio-vom-rbb-931.png', 0, 0, 0, 1, '2011-11-11', 5361, 0, '2017-01-01 00:00:00', 1), \
('AirProgressiveorg', 'Progressive News and Talk radio from around the nation, with shows updated hourly. Live shows coming soon.', 'http://www.airprogressive.org:8000/stream', 'http://www.airprogressive.org/', 'AirProgressiveorg.png', 0, 0, 0, 1, '2011-11-11', 5369, 0, '2017-01-01 00:00:00', 1), \
('SAW-80er', '', 'http://stream.radiosaw.de/saw-80er/mp3-128/tunein/', 'http://www.radiosaw.de/', 'SAW-80er.png', 0, 0, 0, 1, '2011-11-11', 5372, 0, '2017-01-01 00:00:00', 1), \
('Mix-967', '', 'http://stream.radiosaw.de/saw-80er/mp3-128/tunein/', 'http://www.mix96.fm/', 'Mix-967.png', 0, 0, 0, 1, '2011-11-11', 5373, 0, '2017-01-01 00:00:00', 1), \
('Trevor-Nelsons-Soul-Show', '', 'http://stream.radiosaw.de/saw-80er/mp3-128/tunein/', '', 'Trevor-Nelsons-Soul-Show.png', 0, 0, 0, 1, '2011-11-11', 5374, 0, '2017-01-01 00:00:00', 1), \
('Schopp-and-The-Bulldog', '', 'http://stream.radiosaw.de/saw-80er/mp3-128/tunein/', '', 'Schopp-and-The-Bulldog.png', 0, 0, 0, 1, '2011-11-11', 5376, 0, '2017-01-01 00:00:00', 1), \
('Heart-West-Midlands-1007', 'Heart 100.7 with Ed &amp; Rachel at Breakfast covers the West Midlands including Birmingham, Wolverhampton, Coventry, Warwickshire, Worcestershire, South Staffordshire, Shropshire and The Black Countr', 'http://media-ice.musicradio.com/HeartWestMidsMP3', 'http://www.heart.co.uk/westmids/', 'Heart-West-Midlands-1007.png', 0, 0, 0, 1, '2011-11-11', 5409, 0, '2017-01-01 00:00:00', 1), \
('SportsCenter-Sunday', '', 'http://media-ice.musicradio.com/HeartWestMidsMP3', '', 'SportsCenter-Sunday.png', 0, 0, 0, 1, '2011-11-11', 5410, 0, '2017-01-01 00:00:00', 1), \
('City-Dance-Radio', 'City Pop Radio&#169; la emisora online en Valencia (Espa&#241;a) que te har&#225; disfrutar de todos los &#233;xitos del pop latino e internacional desde los 80 hasta la actualidad, con una variad&#23', 'http://listen.radionomy.com/city-dance-radio', 'http://www.citypopradio.es/', 'City-Dance-Radio.png', 0, 0, 0, 1, '2011-11-11', 5414, 0, '2017-01-01 00:00:00', 1), \
('KFXR-1190', '', 'http://listen.radionomy.com/city-dance-radio', 'http://www.dfw1190am.com/', 'KFXR-1190.png', 0, 0, 0, 1, '2011-11-11', 5415, 0, '2017-01-01 00:00:00', 1), \
('181FM-Rock-181', '', 'http://listen.181fm.com/181-rock_128k.mp3', 'http://www.181.fm/', '181FM-Rock-181.png', 0, 0, 0, 1, '2011-11-11', 5417, 0, '2017-01-01 00:00:00', 1), \
('Radio-Schwarze-Welle', '', 'http://www.schwarze-welle.com/play.m3u', 'http://www.schwarze-welle.de/', 'Radio-Schwarze-Welle.png', 0, 0, 0, 1, '2011-11-11', 5423, 0, '2017-01-01 00:00:00', 1), \
('Trinity-Church-Service', '', 'http://www.schwarze-welle.com/play.m3u', '', 'Trinity-Church-Service.png', 0, 0, 0, 1, '2011-11-11', 5424, 0, '2017-01-01 00:00:00', 1), \
('Norea-DK', '', 'http://radio.norea.dk:8080', 'http://www.norea.dk/', 'Norea-DK.png', 0, 0, 0, 1, '2011-11-11', 5483, 0, '2017-01-01 00:00:00', 1), \
('Quincy-Jones', '', 'http://radio.norea.dk:8080', '', 'Quincy-Jones.png', 0, 0, 0, 1, '2011-11-11', 5484, 0, '2017-01-01 00:00:00', 1), \
('Masala-101-1011', '', 'http://mp3.gnxnetwork.com:8002/', 'http://www.masala101.com/', 'Masala-101-1011.png', 0, 0, 0, 1, '2011-11-11', 5510, 0, '2017-01-01 00:00:00', 1), \
('Family-Radio-Network-947', '', 'http://asx.abacast.com/family-east-64.pls', 'http://www.familyradio.com/', 'Family-Radio-Network-947.png', 0, 0, 0, 1, '2011-11-11', 5563, 0, '2017-01-01 00:00:00', 1), \
('La-Movidita-1013', '', 'http://174.122.121.106:8000/movidita-ogg.ogg', 'http://www.cincoradio.com.mx', 'La-Movidita-1013.png', 0, 0, 0, 1, '2011-11-11', 5564, 0, '2017-01-01 00:00:00', 1), \
('181FM-Lite-80s', '', 'http://listen.181fm.com/181-lite80s_128k.mp3', 'http://www.181.fm/', '181FM-Lite-80s.png', 0, 0, 0, 1, '2011-11-11', 5573, 0, '2017-01-01 00:00:00', 1), \
('Laid-Back-Radio', 'Laid Back Radio (LDBK) plays everything from Soul, Funk, Jazz, Psych Rock, Disco and Latin music to Hip Hop and much more.', 'http://listen.radionomy.com/Laid-Back-Radio', 'http://www.laid-back.be/', 'Laid-Back-Radio.png', 0, 0, 0, 1, '2011-11-11', 5591, 0, '2017-01-01 00:00:00', 1), \
('Jamz-FM-979', '', 'http://crystalout.surfernetwork.com:8001/KJMZ_MP3', 'http://www.kjmz.com/', 'Jamz-FM-979.png', 0, 0, 0, 1, '2011-11-11', 5620, 0, '2017-01-01 00:00:00', 1), \
('Gabriel--Dresden', '', 'http://crystalout.surfernetwork.com:8001/KJMZ_MP3', '', 'Gabriel--Dresden.png', 0, 0, 0, 1, '2011-11-11', 5621, 0, '2017-01-01 00:00:00', 1), \
('KMFDM', '', 'http://crystalout.surfernetwork.com:8001/KJMZ_MP3', '', 'KMFDM.png', 0, 0, 0, 1, '2011-11-11', 5622, 0, '2017-01-01 00:00:00', 1), \
('Rick-Ross', '', 'http://crystalout.surfernetwork.com:8001/KJMZ_MP3', '', 'Rick-Ross.png', 0, 0, 0, 1, '2011-11-11', 5623, 0, '2017-01-01 00:00:00', 1), \
('Talk-Radio-1520', '', 'http://stream2.partyzone.nu:8080/', 'http://www.kokcradio.com/', 'Talk-Radio-1520.png', 0, 0, 0, 1, '2011-11-11', 5630, 0, '2017-01-01 00:00:00', 1), \
('Ozomatli', '', 'http://stream2.partyzone.nu:8080/', '', 'Ozomatli.png', 0, 0, 0, 1, '2011-11-11', 5631, 0, '2017-01-01 00:00:00', 1), \
('ESPN-Radio-1340', 'WSTV (1340 AM) is a radio station broadcasting a sports radio format in Steubenville, Ohio. WSTV, although providing a rimshot signal to the Pittsburgh area and within that radio market, provides ESPN', 'http://stream2.partyzone.nu:8080/', '', 'ESPN-Radio-1340.png', 0, 0, 0, 1, '2011-11-11', 5632, 0, '2017-01-01 00:00:00', 1), \
('Viva-Brother', '', 'http://stream2.partyzone.nu:8080/', '', 'Viva-Brother.png', 0, 0, 0, 1, '2011-11-11', 5633, 0, '2017-01-01 00:00:00', 1), \
('Vocal-Legends-on-JAZZRADIOcom', 'Vocal legends like Ella Fitzgerald, Frank Sinatra, Billie Holiday &amp; more.', 'http://listen.jazzradio.com/partner_aac/vocallegends.pls', 'http://www.jazzradio.com/', 'Vocal-Legends-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 5634, 0, '2017-01-01 00:00:00', 1), \
('Carole-King', '', 'http://listen.jazzradio.com/partner_aac/vocallegends.pls', '', 'Carole-King.png', 0, 0, 0, 1, '2011-11-11', 5635, 0, '2017-01-01 00:00:00', 1), \
('WTIX-FM-943', '', 'http://listen.jazzradio.com/partner_aac/vocallegends.pls', 'http://wtixfm.com/', 'WTIX-FM-943.png', 0, 0, 0, 1, '2011-11-11', 5636, 0, '2017-01-01 00:00:00', 1), \
('WKCY-1300', '', 'http://listen.jazzradio.com/partner_aac/vocallegends.pls', 'http://www.wkcyam.com/', 'WKCY-1300.png', 0, 0, 0, 1, '2011-11-11', 5637, 0, '2017-01-01 00:00:00', 1), \
('Counting-Crows', '', 'http://listen.jazzradio.com/partner_aac/vocallegends.pls', '', 'Counting-Crows.png', 0, 0, 0, 1, '2011-11-11', 5638, 0, '2017-01-01 00:00:00', 1), \
('Skaga-FM-887', '', 'http://media.wlmm.dk:80/skagafm', 'http://www.skagafm.dk/', 'Skaga-FM-887.png', 0, 0, 0, 1, '2011-11-11', 5639, 0, '2017-01-01 00:00:00', 1), \
('WAMM-1230', '', 'http://media.wlmm.dk:80/skagafm', 'http://www.wammradio.com/', 'WAMM-1230.png', 0, 0, 0, 1, '2011-11-11', 5640, 0, '2017-01-01 00:00:00', 1), \
('Heart-London-1062', 'Heart 106.2 with Jamie &amp; Harriet at Breakfast covers Greater London', 'http://media-ice.musicradio.com/HeartLondonMP3', 'http://www.heartlondon.co.uk/', 'Heart-London-1062.png', 0, 0, 0, 1, '2011-11-11', 5641, 0, '2017-01-01 00:00:00', 1), \
('In-Our-Time', '', 'http://media-ice.musicradio.com/HeartLondonMP3', '', 'In-Our-Time.png', 0, 0, 0, 1, '2011-11-11', 5642, 0, '2017-01-01 00:00:00', 1), \
('Icehouse', '', 'http://webcast.aph.gov.au/livebroadcasting/asx1/hms935a.asx', '', 'Icehouse.png', 0, 0, 0, 1, '2011-11-11', 5644, 0, '2017-01-01 00:00:00', 1), \
('Radio-Metallica', '', 'http://static.abradio.cz/data/s/34/playlist/metallica32.asx', '', 'Radio-Metallica.png', 0, 0, 0, 1, '2011-11-11', 5647, 0, '2017-01-01 00:00:00', 1), \
('Desree', '', 'http://static.abradio.cz/data/s/34/playlist/metallica32.asx', '', 'Desree.png', 0, 0, 0, 1, '2011-11-11', 5648, 0, '2017-01-01 00:00:00', 1), \
('The-Hollywood-Blue-Flames', '', 'http://static.abradio.cz/data/s/34/playlist/metallica32.asx', '', 'The-Hollywood-Blue-Flames.png', 0, 0, 0, 1, '2011-11-11', 5649, 0, '2017-01-01 00:00:00', 1), \
('Quest-for-Truth', '', 'http://static.abradio.cz/data/s/34/playlist/metallica32.asx', '', 'Quest-for-Truth.png', 0, 0, 0, 1, '2011-11-11', 5650, 0, '2017-01-01 00:00:00', 1), \
('ORS-Radio---Classic-Hip-Hop', 'Modern HipHop owes it&#39;s distinctive style and culture to the songs which built the style from its roots. Take a journey through the history of HipHop, with our choice of tunes from some of history', 'http://www.orsradio.com/wma/classichiphop.asx', 'http://www.orsradio.com/', 'ORS-Radio---Classic-Hip-Hop.png', 0, 0, 0, 1, '2011-11-11', 5662, 0, '2017-01-01 00:00:00', 1), \
('Hawksbee-and-Jacobs', '', 'http://www.orsradio.com/wma/classichiphop.asx', '', 'Hawksbee-and-Jacobs.png', 0, 0, 0, 1, '2011-11-11', 5663, 0, '2017-01-01 00:00:00', 1), \
('Mixnation-Radio-1045', 'MixNation ist einzigartig und spielt Re-Mixe aus dem Bereich Pop, Rock, R&#39;n&#39;B und Hip-Hop.', 'http://sc1.netstreamer.net:8090', 'http://www.mixnation.de/', 'Mixnation-Radio-1045.png', 0, 0, 0, 1, '2011-11-11', 6282, 0, '2017-01-01 00:00:00', 1), \
('Wings', '', 'http://sc1.netstreamer.net:8090', '', 'Wings.png', 0, 0, 0, 1, '2011-11-11', 6283, 0, '2017-01-01 00:00:00', 1), \
('Ramones', '', 'http://www.913thesummit.com/audio/hq.m3u', '', 'Ramones.png', 0, 0, 0, 1, '2011-11-11', 6302, 0, '2017-01-01 00:00:00', 1), \
('Mix-1023', '', 'http://icecast.arn.com.au/1023.aac.m3u', 'http://www.mix1023.com.au/', 'Mix-1023.png', 0, 0, 0, 1, '2011-11-11', 6305, 0, '2017-01-01 00:00:00', 1), \
('Power-1035', '', 'http://crystalout.surfernetwork.com:8001/KVSP_MP3', 'http://www.kvsp.com/', 'Power-1035.png', 0, 0, 0, 1, '2011-11-11', 6307, 0, '2017-01-01 00:00:00', 1), \
('Gospel-Caravan', '', 'http://crystalout.surfernetwork.com:8001/KVSP_MP3', '', 'Gospel-Caravan.png', 0, 0, 0, 1, '2011-11-11', 6308, 0, '2017-01-01 00:00:00', 1), \
('Radio-88---Retro-88-954', '', 'http://stream1.radio88.hu:8300/', 'http://www.radio88.hu/', 'Radio-88---Retro-88-954.png', 0, 0, 0, 1, '2011-11-11', 6332, 0, '2017-01-01 00:00:00', 1), \
('Radio-Schleswig-Holstein-1024', '', 'http://stream.hoerradar.de:80/rsh128', 'http://www.rsh.de/', 'Radio-Schleswig-Holstein-1024.png', 0, 0, 0, 1, '2011-11-11', 6344, 0, '2017-01-01 00:00:00', 1), \
('1067-The-Beat', '', 'http://stream.hoerradar.de/rsh128.m3u', 'http://www.1067thebeat.com', '1067-The-Beat.png', 0, 0, 0, 1, '2011-11-11', 6345, 0, '2017-01-01 00:00:00', 1), \
('The-Morning-Coffee-Mix', 'The Morning Coffee Mix plays a mix of jazz, Top40 and light rock', 'http://sc9.shoutcaststreaming.us:9520/', 'http://www.themorningcoffeemix.com/', 'The-Morning-Coffee-Mix.png', 0, 0, 0, 1, '2011-11-11', 6373, 0, '2017-01-01 00:00:00', 1), \
('101X-1015', '', 'http://sc9.shoutcaststreaming.us:9520/', 'http://www.krox.com/', '101X-1015.png', 0, 0, 0, 1, '2011-11-11', 6374, 0, '2017-01-01 00:00:00', 1), \
('Spirit-FM-905', '', 'http://live.str3am.com:2670', 'http://www.spiritfm905.com/', 'Spirit-FM-905.png', 0, 0, 0, 1, '2011-11-11', 6430, 0, '2017-01-01 00:00:00', 1), \
('Radio-Truckerladen', '', 'http://stream.laut.fm/truckerladen', 'http://www.truckerladen.de/', 'Radio-Truckerladen.png', 0, 0, 0, 1, '2011-11-11', 6456, 0, '2017-01-01 00:00:00', 1), \
('Silver-Convention', '', 'http://stream.laut.fm/truckerladen', '', 'Silver-Convention.png', 0, 0, 0, 1, '2011-11-11', 6457, 0, '2017-01-01 00:00:00', 1), \
('Rachel-Platten', '', 'http://stream.laut.fm/truckerladen', '', 'Rachel-Platten.png', 0, 0, 0, 1, '2011-11-11', 6458, 0, '2017-01-01 00:00:00', 1), \
('Den-Gale-Pose', '', 'http://stream.laut.fm/truckerladen', '', 'Den-Gale-Pose.png', 0, 0, 0, 1, '2011-11-11', 6459, 0, '2017-01-01 00:00:00', 1), \
('Lasgo', '', 'http://stream.laut.fm/truckerladen', '', 'Lasgo.png', 0, 0, 0, 1, '2011-11-11', 6460, 0, '2017-01-01 00:00:00', 1), \
('KINO-1230', '', 'http://stream.laut.fm/truckerladen', '', 'KINO-1230.png', 0, 0, 0, 1, '2011-11-11', 6461, 0, '2017-01-01 00:00:00', 1), \
('RADIO-fresh80s', 'Wir verstehen uns als Spartensender f&#252;r die Musik der Achtziger. Unser Fokus richtet sich hierbei auf ausgefallene und schon in Vergessenheit geratene Musikst&#252;cke dieses Jahrzehnts. Italo-Di', 'http://www.fresh80s.de/listen.m3u', 'http://www.fresh80s.de/', 'RADIO-fresh80s.png', 0, 0, 0, 1, '2011-11-11', 6462, 0, '2017-01-01 00:00:00', 1), \
('Boot-Shoppe', '', 'http://www.fresh80s.de/listen.m3u', '', 'Boot-Shoppe.png', 0, 0, 0, 1, '2011-11-11', 6463, 0, '2017-01-01 00:00:00', 1), \
('Radio-Torino-International-900', 'Radio Torino International - La Radio di Torino per la comunit&#224; Romena.', 'http://stream15.top-ix.org:80/radiotorinointernational', 'http://www.torinointernational.com/', 'Radio-Torino-International-900.png', 0, 0, 0, 1, '2011-11-11', 6464, 0, '2017-01-01 00:00:00', 1), \
('RadioSky-Music-Jazz', 'WebRadio essentially musical, styles in: Jazz, Vocal Jazz, Blues, Swing, BigBand, Latin Jazz, Caribbean, Brazil...', 'http://listen.radionomy.com/radiosky-music', 'http://radiosky-music.playtheradio.com', 'RadioSky-Music-Jazz.png', 0, 0, 0, 1, '2011-11-11', 6611, 0, '2017-01-01 00:00:00', 1), \
('Radio-Beograd-202-1018', '', 'http://rts.ipradio.rs:8006', 'http://www.radiobeograd.rs/', 'Radio-Beograd-202-1018.png', 0, 0, 0, 1, '2011-11-11', 6667, 0, '2017-01-01 00:00:00', 1), \
('Felger-and-Massarotti', '', 'http://media-ice.musicradio.com/HeartOxfordshireMP3', '', 'Felger-and-Massarotti.png', 0, 0, 0, 1, '2011-11-11', 6735, 0, '2017-01-01 00:00:00', 1), \
('XFM-955', '', 'http://stream.xfm.dk:8000/', 'http://www.xfm.dk/', 'XFM-955.png', 0, 0, 0, 1, '2011-11-11', 6760, 0, '2017-01-01 00:00:00', 1), \
('90s-Party', 'Hits des annes 90', 'http://listen.radionomy.com/90-s-party', 'http://www.90-s-party.com/', '90s-Party.png', 0, 0, 1, 1, '2011-11-11', 6761, 0, '2011-10-14 15:57:11', 1), \
('WKNC-FM-881', 'WKNC is student-run non-commercial radio from NC State University. We pride ourselves on our alternative programming of indie rock, electronic, metal and underground hip-hop and have been named &quot;', 'http://wknc.sma.ncsu.edu:8000/wknchq', 'http://wknc.org/', 'WKNC-FM-881.png', 0, 0, 0, 1, '2011-11-11', 6793, 0, '2017-01-01 00:00:00', 1);";

const char *radio_station_setupsql17="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('New-Years-Day', '', 'http://radio.sunradio.ru/children64', '', 'New-Years-Day.png', 0, 0, 0, 1, '2011-11-11', 6844, 0, '2017-01-01 00:00:00', 1), \
('Discover-The-Word', '', 'http://radio.sunradio.ru/children64', '', 'Discover-The-Word.png', 0, 0, 0, 1, '2011-11-11', 6845, 0, '2017-01-01 00:00:00', 1), \
('The-Corrs', '', 'http://radio.sunradio.ru/children64', '', 'The-Corrs.png', 0, 0, 0, 1, '2011-11-11', 6846, 0, '2017-01-01 00:00:00', 1), \
('KSTY-1045', '', 'http://radio.sunradio.ru/children64', '', 'KSTY-1045.png', 0, 0, 0, 1, '2011-11-11', 6847, 0, '2017-01-01 00:00:00', 1), \
('Melissa-Etheridge', '', 'http://radio.sunradio.ru/children64', '', 'Melissa-Etheridge.png', 0, 0, 0, 1, '2011-11-11', 6848, 0, '2017-01-01 00:00:00', 1), \
('ESPN-1410', '', 'http://radio.sunradio.ru/children64', 'http://www.wner1410.com/', 'ESPN-1410.png', 0, 0, 0, 1, '2011-11-11', 6849, 0, '2017-01-01 00:00:00', 1), \
('Vanessa-Feltz', '', 'http://radio.sunradio.ru/children64', '', 'Vanessa-Feltz.png', 0, 0, 0, 1, '2011-11-11', 6850, 0, '2017-01-01 00:00:00', 1), \
('Sportsradio-950-ESPN', '', 'http://radio.sunradio.ru/children64', 'http://www.sportsradio950ESPN.com/', 'Sportsradio-950-ESPN.png', 0, 0, 0, 1, '2011-11-11', 6851, 0, '2017-01-01 00:00:00', 1), \
('WTUL-915', 'WTUL is a progressive/alternative FM radio outlet in New Orleans, Louisiana. The station, operated primarily by students of Tulane University, offers a mix of cutting-edge progressive, electronica, cl', 'http://129.81.156.83:8000/listen', 'http://www.wtulneworleans.com/', 'WTUL-915.png', 0, 0, 0, 1, '2011-11-11', 6854, 0, '2017-01-01 00:00:00', 1), \
('Hot-Panda', '', 'http://129.81.156.83:8000/listen', '', 'Hot-Panda.png', 0, 0, 0, 1, '2011-11-11', 6861, 0, '2017-01-01 00:00:00', 1), \
('2M-Club-Radio', '', 'http://listen.radionomy.com/2mclub', 'http://2mclub.fr/', '2M-Club-Radio.png', 0, 0, 0, 1, '2011-11-11', 6869, 0, '2017-01-01 00:00:00', 1), \
('KCOU-881', 'The University of Missouri&#39;s completely student-run station.  Broadcasting since 1963, it features the best in new and emerging music from a variety of genres along with a focus on Mizzou news and', 'http://radio.kcou.fm:8180', 'http://kcou.fm/', 'KCOU-881.png', 0, 0, 0, 1, '2011-11-11', 6877, 0, '2017-01-01 00:00:00', 1), \
('KROE-930', 'Sheridan Media:  Sheridan and Northern Wyoming news sports weather and community information. Radio stations KZWY, KYTI, KWYO, KROE Sheridan WY', 'http://www.sheridanmedia.com/audio/kroe/wmstream.asx', 'http://www.sheridanmedia.com/kroe', 'KROE-930.png', 0, 0, 0, 1, '2011-11-11', 6921, 0, '2017-01-01 00:00:00', 1), \
('EW-Virtual-Radio', '', 'http://38.96.148.37:6410/', 'http://www.ewvirtualdesign.com/', 'EW-Virtual-Radio.png', 0, 0, 0, 1, '2011-11-11', 6922, 0, '2017-01-01 00:00:00', 1), \
('Luis-Miguel', '', 'http://broadcast.infomaniak.net:80/energyitaly-high.mp3', '', 'Luis-Miguel.png', 0, 0, 0, 1, '2011-11-11', 6926, 0, '2017-01-01 00:00:00', 1), \
('Radio-Viet-Nam-Hai-Ngoai', '', 'http://vpr.serverroom.us:7492/', 'http://www.radiohaingoai.com/', 'Radio-Viet-Nam-Hai-Ngoai.png', 0, 0, 0, 1, '2011-11-11', 6928, 0, '2017-01-01 00:00:00', 1), \
('987-KUPL', 'New Country 98.7 KUPL has been Portland&#39;s #1 choice for country music for over two decades.', 'http://stm1.rthk.hk/radio1', 'http://www.kupl.com', '987-KUPL.png', 0, 0, 0, 1, '2011-11-11', 6954, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Nordjylland', '', 'http://live-icy.gss.dr.dk:8000/A/A10H.mp3', 'http://www.dr.dk/p4/nord/', 'DR-P4-Nordjylland-981.png', 0, 0, 0, 1, '2011-11-11', 7000, 0, '2017-01-01 00:00:00', 1), \
('All-Hitz-977', 'WLER-FM is an Adult Contemporary radio station that officially can be heard in Butler County, Pennsylvania, but can also be heard in parts of northern Allegheny County, including Pittsburgh.', 'http://163.172.126.155:80/1', 'http://www.wlerfm.com/', 'All-Hitz-977.png', 0, 0, 1, 1, '2011-11-11', 7298, 0, '2011-10-15 20:11:51', 1), \
('Hole', '', 'http://163.172.126.155:80/1', '', 'Hole.png', 0, 0, 0, 1, '2011-11-11', 7299, 0, '2017-01-01 00:00:00', 1), \
('Ganga', '', 'http://163.172.126.155:80/1', '', 'Ganga.png', 0, 0, 0, 1, '2011-11-11', 7300, 0, '2017-01-01 00:00:00', 1), \
('KKEE-1230', '', 'http://163.172.126.155:80/1', 'http://www.kkee1230.com/', 'KKEE-1230.png', 0, 0, 0, 1, '2011-11-11', 7301, 0, '2017-01-01 00:00:00', 1), \
('DR Klassisk', '', 'http://live-icy.gss.dr.dk:8000/A/A04H.mp3', 'http://www.dr.dk/radio/', 'DR-Klassisk.png', 0, 0, 0, 1, '2011-11-11', 7331, 0, '2017-01-01 00:00:00', 1), \
('Dragonland-Radio-960', 'Das Dragonland-Radio sorgt mit der Partynight, dem Kaffeeklatsch, dem Spa&#223; am Mittag und anderen Sendungen f&#252;r Ihre musikalische Unterhaltung.', 'http://www.soulfulbits.com/files/streaming/soulfulbits-radio.m3u', 'http://www.dragonland-radio.de/', 'Dragonland-Radio-960.png', 0, 0, 0, 1, '2011-11-11', 7777, 0, '2017-01-01 00:00:00', 0), \
('Patriotscom-Radio', 'Official online radio station of the New England Patriots, featuring shows such as PFW in Progress, The Pete Sheppard Show, Patriots Playbook, Patriots Monday, Patriots Friday, Fantasy Lowe Down, and ', 'http://www.soulfulbits.com/files/streaming/soulfulbits-radio.m3u', 'http://www.patriots.com/', 'Patriotscom-Radio.png', 0, 0, 0, 1, '2011-11-11', 7780, 0, '2017-01-01 00:00:00', 0), \
('Mietta', '', 'http://www.soulfulbits.com/files/streaming/soulfulbits-radio.m3u', '', 'Mietta.png', 0, 0, 0, 1, '2011-11-11', 7785, 0, '2017-01-01 00:00:00', 0), \
('Sandra', '', 'http://listen.radionomy.com/M-B-C-Radio', '', 'Sandra.png', 0, 0, 0, 1, '2011-11-11', 7917, 0, '2017-01-01 00:00:00', 1), \
('Nummer-1-Oldies', 'Klasse Musik zum Entspannen oder Mitsingen', 'http://stream.laut.fm/nummer_1_oldies', 'http://www.laut.fm/nummer_1_oldies', 'Nummer-1-Oldies.png', 0, 0, 0, 1, '2011-11-11', 7962, 0, '2017-01-01 00:00:00', 1), \
('Kevin-Eubanks', '', 'http://stream.laut.fm/nummer_1_oldies', '', 'Kevin-Eubanks.png', 0, 0, 0, 1, '2011-11-11', 7963, 0, '2017-01-01 00:00:00', 1), \
('181FM-Old-Love-Songs)', '', 'http://listen.181fm.com/181-heart_128k.mp3', 'http://www.181.fm/', '', 0, 0, 0, 1, '2011-11-11', 8003, 0, '2017-01-01 00:00:00', 1), \
('Blues-Club', '24h Rock &amp; Blues von Heavy bis Soft! Rocking The Blues &amp; Hot Guitars: Rock, Classic - Rock, Blues, Instrumentals!', 'http://stream.laut.fm/bluesclub', 'http://www.laut.fm/bluesclub', 'Blues-Club.png', 0, 0, 0, 1, '2011-11-11', 8009, 0, '2017-01-01 00:00:00', 1), \
('Radio-Skive-1043', '', 'http://netradio.radioskive.dk', 'http://www.radioskive.dk/', 'Radio-Skive-1043.png', 0, 0, 0, 1, '2011-11-11', 8034, 0, '2017-01-01 00:00:00', 1), \
('Unshackled', '', 'http://netradio.radioskive.dk', '', 'Unshackled.png', 0, 0, 0, 1, '2011-11-11', 8035, 0, '2017-01-01 00:00:00', 1), \
('Celtica-Radio', 'From South Wales', 'http://51.255.235.165:5232/stream', 'http://www.celticaradio.com/', 'Celtica-Radio.png', 0, 0, 0, 1, '2011-11-11', 8114, 0, '2017-01-01 00:00:00', 1), \
('Radio-K-770', 'Radio K is the award-winning student-run radio station of the University of Minnesota, playing an eclectic variety of independent music both old and new.  <BR><BR>Radio K educates students, breaks gro', 'http://radiokstreams.cce.umn.edu:8128', 'http://radiok.cce.umn.edu/', 'Radio-K-770.png', 0, 0, 0, 1, '2011-11-11', 8119, 0, '2017-01-01 00:00:00', 1), \
('Ryan-Adams', '', 'http://streaming.radionomy.com/100-HIT-radio', '', 'Ryan-Adams.png', 0, 0, 0, 1, '2011-11-11', 8124, 0, '2017-01-01 00:00:00', 1), \
('Cosmic-Gate', '', 'http://sc1.mainstreamnetwork.com:9076/', '', 'Cosmic-Gate.png', 0, 0, 0, 1, '2011-11-11', 8169, 0, '2017-01-01 00:00:00', 1), \
('The-Mix', '', 'http://streaming08.mit.ovea.com/medi1', '', 'The-Mix.png', 0, 0, 0, 1, '2011-11-11', 8183, 0, '2017-01-01 00:00:00', 1), \
('Hit-Radio-N1-929', 'Wir spielen eure Lieblingshits.', 'http://webradio.hitradion1.de:8000', 'http://www.hitradion1.de/', 'Hit-Radio-N1-929.png', 0, 0, 0, 1, '2011-11-11', 8184, 0, '2017-01-01 00:00:00', 1), \
('Heart-Bristol-963', 'Heart 96.3 with Bush, Troy and Paulina at breakfast covers Bristol.', 'http://media-ice.musicradio.com/HeartBristolMP3', 'http://www.heart.co.uk/bristol/', 'Heart-Bristol-963.png', 0, 0, 0, 1, '2011-11-11', 8253, 0, '2017-01-01 00:00:00', 1), \
('Goa-Trance-Chillout', '', 'http://komplex2.psyradio.org:8020/', 'http://www.goa-trance.de/', 'Goa-Trance-Chillout.png', 0, 0, 0, 1, '2011-11-11', 8331, 0, '2017-01-01 00:00:00', 1), \
('Ocean-Beach-Radio', 'It&#39;s only a little bit of Rock &#39;N Roll, and a lot of Americana and Country, Folk, Blues, Jazz and some other stuff......', 'http://ophanim.net:7130/', 'http://www.oceanbeachradio.com/', 'Ocean-Beach-Radio.png', 0, 0, 0, 1, '2011-11-11', 8412, 0, '2017-01-01 00:00:00', 1), \
('Mega-Rock-1055', '', 'http://ophanim.net:7130/', 'http://www.megarock.fm/', 'Mega-Rock-1055.png', 0, 0, 0, 1, '2011-11-11', 8413, 0, '2017-01-01 00:00:00', 1), \
('BestRadio', 'BestRadio', 'http://listen.radionomy.com/bestradio', 'http://www.best-radio.net/', 'BestRadio.png', 0, 0, 0, 1, '2011-11-11', 8428, 0, '2017-01-01 00:00:00', 1), \
('WOAI-1200', '', 'http://listen.radionomy.com/bestradio', 'http://www.1200woairadio.com/', 'WOAI-1200.png', 0, 0, 0, 1, '2011-11-11', 8429, 0, '2017-01-01 00:00:00', 1), \
('The-National', '', 'http://listen.radionomy.com/bestradio', '', 'The-National.png', 0, 0, 0, 1, '2011-11-11', 8430, 0, '2017-01-01 00:00:00', 1), \
('181FM-The-Mix', '', 'http://listen.181fm.com/181-themix_128k.mp3', 'http://www.181.fm/', '181FM-The-Mix.png', 0, 0, 0, 1, '2011-11-11', 8435, 0, '2017-01-01 00:00:00', 1), \
('Mayhem-in-the-AM', '', 'http://listen.radionomy.com/Play-Misty-for-Me', '', 'Mayhem-in-the-AM.png', 0, 0, 0, 1, '2011-11-11', 8440, 0, '2017-01-01 00:00:00', 1), \
('K-TIP-Radio-1450', '', 'http://listen.radionomy.com/alternativeradio', 'http://www.ktip.com/', 'K-TIP-Radio-1450.png', 0, 0, 0, 1, '2011-11-11', 8450, 0, '2017-01-01 00:00:00', 1), \
('KPOO-895', 'KPOO is an independent, listener-sponsored noncommercial station. KPOO is an African-American owned and operated noncommercial radio station. KPOO broadcasts 24 hours a day on 160 watts, with a radiat', 'http://amber.streamguys.com:5220/xstream', 'http://www.kpoo.com/', 'KPOO-895.png', 0, 0, 0, 1, '2011-11-11', 8481, 0, '2017-01-01 00:00:00', 1), \
('KKOL-1300', '', 'http://media-ice.musicradio.com/HeartEssexMP3', 'http://kol.townhall.com/', 'KKOL-1300.png', 0, 0, 0, 1, '2011-11-11', 8550, 0, '2017-01-01 00:00:00', 1), \
('Golden-Earring', '', 'http://stream.ir-media-tec.com/radioteddy.mp3', '', 'Golden-Earring.png', 0, 0, 0, 1, '2011-11-11', 8553, 0, '2017-01-01 00:00:00', 1), \
('181FM-The-Point', '', 'http://listen.181fm.com/181-thepoint_128k.mp3', 'http://www.181.fm/', '181FM-The-Point.png', 0, 0, 0, 1, '2011-11-11', 8565, 0, '2017-01-01 00:00:00', 1), \
('Energy-Radio-Belgrade', '', 'http://www.energyradio.rs:7799', 'http://www.energyradio.rs/', 'Energy-Radio-Belgrade.png', 0, 0, 0, 1, '2011-11-11', 8566, 0, '2017-01-01 00:00:00', 1), \
('WSIE-887', 'Serving the Greater St. Louis Area and Southwestern Illinois', 'http://streaming.siue.edu:8000/wsie.mp3', 'http://www.wsie.com', 'WSIE-887.png', 0, 0, 0, 1, '2011-11-11', 8593, 0, '2017-01-01 00:00:00', 1), \
('Kurt-Elling', '', 'http://streaming.siue.edu:8000/wsie.mp3', '', 'Kurt-Elling.png', 0, 0, 0, 1, '2011-11-11', 8595, 0, '2017-01-01 00:00:00', 1), \
('96sterreich-1-920', '', 'http://mp3stream3.apasf.apa.at:8000', 'http://oe1.orf.at/', '', 0, 0, 0, 1, '2011-11-11', 8603, 0, '2017-01-01 00:00:00', 1), \
('KOZI-FM-935', 'Serving all of North Central Washington with the most up to date news &amp; information alongside the best mix of music.', 'http://asx.abacast.com/icicle_broadcasting-kozi-64.asx', 'http://www.kozi.com/', 'KOZI-FM-935.png', 0, 0, 0, 1, '2011-11-11', 8614, 0, '2017-01-01 00:00:00', 1), \
('The-Voice-1049', 'Danmarks st&#248;rste ungdomsbrand', 'http://stream.voice.dk/voice128', 'http://www.thevoice.dk/', 'The-Voice-1049.png', 0, 0, 0, 1, '2011-11-11', 8615, 0, '2017-01-01 00:00:00', 1), \
('KAT-103-1037', '', 'http://stream.voice.dk/voice128', 'http://www.thekat.com/', 'KAT-103-1037.png', 0, 0, 0, 1, '2011-11-11', 8617, 0, '2017-01-01 00:00:00', 1), \
('WAYV-951', '', 'http://crystalout.surfernetwork.com:8001/WAYV_MP3', 'http://www.951wayv.com/', 'WAYV-951.png', 0, 0, 0, 1, '2011-11-11', 8674, 0, '2017-01-01 00:00:00', 1), \
('Spirit-Catholic-Radio-1027', '', 'http://208.43.207.220', 'http://www.kvss.com/', 'Spirit-Catholic-Radio-1027.png', 0, 0, 0, 1, '2011-11-11', 8683, 0, '2017-01-01 00:00:00', 1), \
('John-Scofield', '', 'http://audiostreamen.nl:8062', '', 'John-Scofield.png', 0, 0, 0, 1, '2011-11-11', 8744, 0, '2017-01-01 00:00:00', 1), \
('Horseshoes-and-Hand-Grenades', '', 'http://shoutcast.wrn.org:8012', '', 'Horseshoes-and-Hand-Grenades.png', 0, 0, 0, 1, '2011-11-11', 8767, 0, '2017-01-01 00:00:00', 1), \
('80-EXITS', '', 'http://listen.radionomy.com/80-exits', 'http://www.hospitaletfm.com/80exits.php', '80-EXITS.png', 0, 0, 0, 1, '2011-11-11', 8798, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Midt Vest', '', 'http://live-icy.gss.dr.dk:8000/A/A09H.mp3', 'http://www.dr.dk/p4/vest/', 'DR-P4-Midt--Vest-985.png', 0, 0, 0, 1, '2011-11-11', 8807, 0, '2017-01-01 00:00:00', 1), \
('181FM-The-Office', '', 'http://listen.181fm.com/181-office_128k.mp3', 'http://www.181.fm/', '181FM-The-Office.png', 0, 0, 0, 1, '2011-11-11', 8847, 0, '2017-01-01 00:00:00', 1), \
('The-Planet-967', '', 'http://crystalout.surfernetwork.com:8001/WXZO_MP3', 'http://www.theplanet967.com/', 'The-Planet-967.png', 0, 0, 0, 1, '2011-11-11', 8912, 0, '2017-01-01 00:00:00', 1), \
('NRK-P1-Hordaland-891', '', 'http://lyd.nrk.no/nrk_radio_p1_hordaland_mp3_h', 'http://www.nrk.no/hordaland/', 'NRK-P1-Hordaland-891.png', 0, 0, 0, 1, '2011-11-11', 8947, 0, '2017-01-01 00:00:00', 1), \
('Radio-Expres', '', 'http://85.248.7.162:8000/96.mp3', 'http://www.expres.sk/', 'Radio-Expres.png', 0, 0, 0, 1, '2011-11-11', 9013, 0, '2017-01-01 00:00:00', 1), \
('Lester-Young', '', 'http://media-ice.musicradio.com/HeartKentMP3', '', 'Lester-Young.png', 0, 0, 0, 1, '2011-11-11', 9028, 0, '2017-01-01 00:00:00', 1), \
('Energy-FM-Australia-878', 'Dance music radio station broadcasting 24 hours a day on 87.8 FM in Hobart and streaming live online.', 'http://s3.viastreaming.net:8502', 'http://www.energyfm.com.au/', 'Energy-FM-Australia-878.png', 0, 0, 0, 1, '2011-11-11', 9060, 0, '2017-01-01 00:00:00', 1), \
('Phoenix-Hospital-Radio', 'Phoenix Hospital Radio provides a broadcasting service to the Queen&#39;s Hospital in Burton on Trent.', 'http://studio.phoenixhospitalradio.com:8000/stream.mp3', 'http://www.phoenixhospitalradio.com/', 'Phoenix-Hospital-Radio.png', 0, 0, 0, 1, '2011-11-11', 9094, 0, '2017-01-01 00:00:00', 1), \
('Ida-Corr', '', 'http://studio.phoenixhospitalradio.com:8000/stream.mp3', '', 'Ida-Corr.png', 0, 0, 0, 1, '2011-11-11', 9096, 0, '2017-01-01 00:00:00', 1), \
('WELI-960', '', 'http://studio.phoenixhospitalradio.com:8000/stream.mp3', 'http://www.weliam.com/', 'WELI-960.png', 0, 0, 0, 1, '2011-11-11', 9097, 0, '2017-01-01 00:00:00', 1), \
('Puls-FM-1033', '', 'http://firewall.pulsradio.com', 'http://www.pulsfm.ru/', 'Puls-FM-1033.png', 0, 0, 0, 1, '2011-11-11', 9098, 0, '2017-01-01 00:00:00', 1), \
('WETA-909', 'WETA&#39;s mission is to produce and broadcast programs of intellectual integrity and cultural merit that recognize viewers&#39; and listeners&#39; intelligence, curiosity and interest in the world ar', 'http://stream.weta.org:8000', 'http://www.weta.org/fm/', 'WETA-909.png', 0, 0, 0, 1, '2011-11-11', 9099, 0, '2017-01-01 00:00:00', 1), \
('Antenne-Bayern-Lovesongs', '', 'http://www.antenne.de/webradio/channels/lovesongs.aac.pls', 'http://www.antenne.de/', 'Antenne-Bayern-Lovesongs.png', 0, 0, 0, 1, '2011-11-11', 9101, 0, '2017-01-01 00:00:00', 1), \
('NERadio-Sweden', 'NERadio og Hardstyle.nu eies og drives av NElite Music AS.<BR>NElite Music AS driver flere web radio stasjoner p&#229; forskjellige spr&#229;k og lyttere rundt om i verdenm ofte med live radiovert p&#', 'http://007.neradio.com:9000/', 'http://no.neradio.fm/', 'NERadio-Sweden.png', 0, 0, 0, 1, '2011-11-11', 9150, 0, '2017-01-01 00:00:00', 1), \
('Active-Gaydance', 'Night club music all the day 24/7 on RainbowWebradio.com', 'http://listen.radionomy.com/Rainbow-Webradio', 'http://www.rainbowwebradio.com', 'Active-Gaydance.png', 0, 0, 0, 1, '2011-11-11', 9172, 0, '2017-01-01 00:00:00', 1), \
('Antwerpen-fm-1054', '', 'http://streams.lazernet.be:2710/', 'http://www.antwerpen.fm', 'Antwerpen-fm-1054.png', 0, 0, 0, 1, '2011-11-11', 9180, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Sjælland', '', 'http://live-icy.gss.dr.dk:8000/A/A11H.mp3', 'http://www.dr.dk/p4/sjaelland/', 'DR-P4-Sjaelland-975.png', 0, 0, 0, 1, '2011-11-11', 9226, 0, '2017-01-01 00:00:00', 1), \
('Hit-FM-1067', '', 'http://online-hitfm.tavrmedia.ua:7000/HitFM', 'http://www.hitfm.ua/', 'Hit-FM-1067.png', 0, 0, 0, 1, '2011-11-11', 9295, 0, '2017-01-01 00:00:00', 1), \
('Radio-DeeGay', 'Il pi&#249; cool del nostro network con la musica pi&#249; nuova, l&#39;informazione, i programmi in voce e le dirette degli eventi pi&#249; importanti del mondo GLBT italiano.<BR>Le HIT dance e pop p', 'http://ss1.deegay.it:80/', 'http://www.deegay.it/', 'Radio-DeeGay.png', 0, 0, 0, 1, '2011-11-11', 9296, 0, '2017-01-01 00:00:00', 1), \
('KBIM-FM-949', '', 'http://crystalout.surfernetwork.com:8001/KBIM_MP3', 'http://kbim949.com/', 'KBIM-FM-949.png', 0, 0, 0, 1, '2011-11-11', 9302, 0, '2017-01-01 00:00:00', 1), \
('Talk-Radio-1150', '', 'http://crystalout.surfernetwork.com:8001/KBIM_MP3', 'http://www.kcpsradio.com/', 'Talk-Radio-1150.png', 0, 0, 0, 1, '2011-11-11', 9304, 0, '2017-01-01 00:00:00', 1);";






const char *radio_station_setupsql27="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Radio-Caroline-319-Gold', '', 'http://www.rcgoldserver.eu:8192', 'http://www.radiocaroline319.eu/', 'Radio-Caroline-319-Gold.png', 0, 0, 0, 1, '2011-11-11', 9336, 0, '2017-01-01 00:00:00', 1), \
('1069-KZY', '', 'http://www.rcgoldserver.eu:8192', 'http://www.1069kzy.com/', '1069-KZY.png', 0, 0, 0, 1, '2011-11-11', 9338, 0, '2017-01-01 00:00:00', 1), \
('Radio-Free-Americana', '', 'http://rs1.radiostreamer.de:8000', 'http://www.radiofreeamericana.com/', 'Radio-Free-Americana.png', 0, 0, 0, 1, '2011-11-11', 9395, 0, '2017-01-01 00:00:00', 1), \
('Fun-Radio-Dance', '24-hour stream of dance music, from the latest hits to the 90-ies, designed especially for diskomaniakov.', 'http://stream.funradio.sk:8000/dance128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-Dance.png', 0, 0, 0, 1, '2011-11-11', 9449, 0, '2017-01-01 00:00:00', 1), \
('Club--Dance-Radio', '', 'http://listen.radionomy.com/club-dance', 'http://www.clubanddance.com/', 'Club--Dance-Radio.png', 0, 0, 0, 1, '2011-11-11', 9453, 0, '2017-01-01 00:00:00', 1), \
('NRJ-Girl', '', 'http://broadcast.infomaniak.ch/energygirl-high.mp3.m3u', 'http://www.energyzueri.ch/', 'NRJ-Girl.png', 0, 0, 0, 1, '2011-11-11', 9466, 0, '2017-01-01 00:00:00', 1), \
('DR P6 Beat', '', 'http://live-icy.gss.dr.dk:8000/A/A29H.mp3', 'http://www.dr.dk/p6beat/', 'DR-P6-Beat.png', 0, 0, 0, 1, '2011-11-11', 9473, 0, '2017-01-01 00:00:00', 1), \
('WHPK-FM-885', '', 'http://www.whpk.org:8000/mp3', 'http://whpk.uchicago.edu/', 'WHPK-FM-885.png', 0, 0, 0, 1, '2011-11-11', 9479, 0, '2017-01-01 00:00:00', 1), \
('ESPN-Radio-1360', '', 'http://www.whpk.org:8000/mp3', '', 'ESPN-Radio-1360.png', 0, 0, 0, 1, '2011-11-11', 9480, 0, '2017-01-01 00:00:00', 1), \
('The-BBC-London-Boxing-Hour', '', 'http://www.whpk.org:8000/mp3', '', 'The-BBC-London-Boxing-Hour.png', 0, 0, 0, 1, '2011-11-11', 9482, 0, '2017-01-01 00:00:00', 1), \
('Ricky-Martin', '', 'http://crystalout.surfernetwork.com:8001/WMRQ_HD2_MP3', '', 'Ricky-Martin.png', 0, 0, 0, 1, '2011-11-11', 9581, 0, '2017-01-01 00:00:00', 1), \
('Philadelphia-Eagles-at-Buffalo-Bills-Oct-9-2011', '', 'http://crystalout.surfernetwork.com:8001/WMRQ_HD2_MP3', '', 'Philadelphia-Eagles-at-Buffalo-Bills-Oct-9-2011.png', 0, 0, 0, 1, '2011-11-11', 9588, 0, '2017-01-01 00:00:00', 1), \
('WRWH-1350', '', 'http://peace.str3am.com:6070/live', 'http://www.wrwh.com/', 'WRWH-1350.png', 0, 0, 0, 1, '2011-11-11', 9594, 0, '2017-01-01 00:00:00', 1), \
('Retro--Rock--Pop', 'El Grupo Radial M&amp;M y su Emisora Online Retro &#169; Rock &amp; Pop, transmite su se&#241;al desde Lima - Per&#250; para todo el mundo las 24 horas del d&#237;a y durante los 7 d&#237;as de la sem', 'http://listen.radionomy.com/radio-retro-rock--pop', 'http://www.radioretro.comlu.com/', 'Retro--Rock--Pop.png', 0, 0, 0, 1, '2011-11-11', 9595, 0, '2017-01-01 00:00:00', 1), \
('KPCC-893', 'Southern California Public Radio (SCPR) is a member-supported public radio network that operates 89.3 KPCC-FM in Los Angeles and Orange County, 89.1 KUOR-FM in the Inland Empire and 90.3 KPCV in the C', 'http://kpcclive1.publicradio.org', 'http://www.scpr.org/', 'KPCC-893.png', 0, 0, 0, 1, '2011-11-11', 9597, 0, '2017-01-01 00:00:00', 1), \
('Backstreet-Boys', '', 'http://wtmd-ice.streamguys1.com:8000/wtmd', '', 'Backstreet-Boys.png', 0, 0, 0, 1, '2011-11-11', 9612, 0, '2017-01-01 00:00:00', 1), \
('JPHiP-Radio', 'The latest in Jpop, Kpop, Cpop, HiP HoP.', 'http://radio.jphip.com:8800', 'http://jphip.com/radio.html', 'JPHiP-Radio.png', 0, 0, 0, 1, '2011-11-11', 9619, 0, '2017-01-01 00:00:00', 1), \
('Classic-Rap', 'Classic Rap est une Web radio, France.', 'http://listen.radionomy.com/classic-rap', 'http://classicrap.fr/', 'Classic-Rap.png', 0, 0, 0, 1, '2011-11-11', 9622, 0, '2017-01-01 00:00:00', 1), \
('181FM-UK-Top-40', '', 'http://listen.181fm.com/181-uktop40_128k.mp3', 'http://www.181.fm/', '181FM-UK-Top-40.png', 0, 0, 0, 1, '2011-11-11', 9637, 0, '2017-01-01 00:00:00', 1), \
('Aarhus-Studenterradio-987', '', 'http://aasr-stream.iha.dk:8000', 'http://www.aarhusstudenterradio.dk', 'Aarhus-Studenterradio-987.png', 0, 0, 0, 1, '2011-11-11', 9774, 0, '2017-01-01 00:00:00', 1), \
('Smag-på-P3', '', 'http://bhammountain.serverroom.us:4376', '', 'Smag-paa-P3.png', 0, 0, 0, 1, '2011-11-11', 9776, 0, '2017-01-01 00:00:00', 1), \
('Allegro---Jazz', '', 'http://listen.radionomy.com/allegro-jazz', 'http://www.allegro-radio.com/jazz.html', 'Allegro---Jazz.png', 0, 0, 1, 1, '2011-11-11', 9779, 0, '2011-10-15 20:12:11', 1), \
('181FM-Classic-Hits-181', 'Home of The 60&#39;s and 70&#39;s', 'http://listen.181fm.com/181-greatoldies_128k.mp3', 'http://www.181.fm/', '181FM-Classic-Hits-181.png', 0, 0, 0, 1, '2011-11-11', 9800, 0, '2017-01-01 00:00:00', 1), \
('Black-Eyed-Peas', '', 'http://listen.radionomy.com/abcd-black-eyed-peas', 'http://www.abcdeurodance.com', 'ABCD-Black-Eyed-Peas.png', 0, 0, 1, 1, '2011-11-11', 9842, 0, '2011-10-14 15:59:07', 1), \
('Hits-My-Music', 'e meilleur de la musique des ann&#233;es 90 &#224; aujourd&#39;hui.', 'http://listen.radionomy.com/hit-s-my-music', 'http://www.hitsmymusic.com/', 'Hits-My-Music.png', 0, 0, 0, 1, '2011-11-11', 9844, 0, '2017-01-01 00:00:00', 1), \
('the-TEAM-1045', '', 'http://listen.radionomy.com/hit-s-my-music', 'http://www.wtmm.com/', 'the-TEAM-1045.png', 0, 0, 0, 1, '2011-11-11', 9847, 0, '2017-01-01 00:00:00', 1), \
('Alternate-USA', 'Today&#39;s best alternative songs from USA - live 24/7 !', 'http://listen.radionomy.com/alternate-usa', 'http://alternateusa.com/', 'Alternate-USA.png', 0, 0, 0, 1, '2011-11-11', 9867, 0, '2017-01-01 00:00:00', 1), \
('Radio-Health-Journal', '', 'http://stream.nfowars.net/', '', 'Radio-Health-Journal.png', 0, 0, 0, 1, '2011-11-11', 9894, 0, '2017-01-01 00:00:00', 1), \
('Energy-rich-1009', 'Radio Energy', 'http://broadcast.infomaniak.net/energyzuerich-high.mp3', 'http://www.energyzueri.ch/', '', 0, 0, 0, 1, '2011-11-11', 9923, 0, '2017-01-01 00:00:00', 1), \
('KPSL-FM-965', '', 'http://broadcast.infomaniak.net/energyzuerich-high.mp3', '', 'KPSL-FM-965.png', 0, 0, 0, 1, '2011-11-11', 9924, 0, '2017-01-01 00:00:00', 1), \
('SAW-70er', '', 'http://stream.radiosaw.de/saw-70er/mp3-128/tunein/', 'http://www.radiosaw.de/', 'SAW-70er.png', 0, 0, 0, 1, '2011-11-11', 10036, 0, '2017-01-01 00:00:00', 1), \
('Juli', '', 'http://listen.radionomy.com/abc-lounge', '', 'Juli.png', 0, 0, 0, 1, '2011-11-11', 10125, 0, '2017-01-01 00:00:00', 1), \
('Allectro-Radio', 'We play the best Electro, House, Dubstep, Dance and Drum and Bass.', 'http://listen.radionomy.com/allectro-radio', 'http://radio.rubensworks.net', 'Allectro-Radio.png', 0, 0, 10, 1, '2011-11-11', 10212, 0, '2011-10-15 20:12:08', 1), \
('949-TNT', '', 'http://listen.radionomy.com/allectro-radio', 'http://www.wtntfm.com/', '949-TNT.png', 0, 0, 1, 1, '2011-11-11', 10215, 0, '2011-10-14 15:57:35', 1), \
('Kiss-FM-1059', '', 'http://webcast.wonc.org:8000/wonclive-128s', 'http://www.kissdetroit.com/', 'Kiss-FM-1059.png', 0, 0, 0, 1, '2011-11-11', 10238, 0, '2017-01-01 00:00:00', 1), \
('WSOU-895', '', 'http://crystalout.surfernetwork.com:8001/WSOU_MP3', 'http://www.wsou.net/', 'WSOU-895.png', 0, 0, 0, 1, '2011-11-11', 10239, 0, '2017-01-01 00:00:00', 1), \
('Radio-Generation-Do', '', 'http://listen.radionomy.com/generationdo', '', 'Radio-Generation-Do.png', 0, 0, 0, 1, '2011-11-11', 10246, 0, '2017-01-01 00:00:00', 1), \
('The-Music-Goes-Round', '', 'http://listen.radionomy.com/generationdo', '', 'The-Music-Goes-Round.png', 0, 0, 0, 1, '2011-11-11', 10248, 0, '2017-01-01 00:00:00', 1), \
('ShoutedFM-mthBlack', '', 'http://black.mthn.net:8300', 'http://www.shouted.fm/index.php?area=Black', 'ShoutedFM-mthBlack.png', 0, 0, 0, 1, '2011-11-11', 10260, 0, '2017-01-01 00:00:00', 1), \
('WCRN-830', '', 'http://www.wcrnradio.com/listen.pls', 'http://www.wcrnradio.com/', 'WCRN-830.png', 0, 0, 0, 1, '2011-11-11', 10264, 0, '2017-01-01 00:00:00', 1), \
('RPR1-Season', 'Auf diesem Musikkanal laufen nur die Titel, die zur jeweiligen Jahreszeit passen. Sommerhits im Urlaub, Stimmungsvolles zur Weihnachstzeit, N&#228;rrisches zum Fasching...', 'http://217.151.151.90/stream6', 'http://www.rpr1.de/', 'RPR1-Season.png', 0, 0, 0, 1, '2011-11-11', 10268, 0, '2017-01-01 00:00:00', 1), \
('Antiques--Collectibles-Show', '', 'http://arsradiotv.com:8005', '', 'Antiques--Collectibles-Show.png', 0, 0, 0, 1, '2011-11-11', 10287, 0, '2017-01-01 00:00:00', 1), \
('Rai-webradio-6', 'la web radio dedicata alla storia', 'http://mediapolis.rai.it/relinker/relinkerServlet.htm?cont=174078', 'http://www.wr6.rai.it/', 'Rai-webradio-6.png', 0, 0, 0, 1, '2011-11-11', 10311, 0, '2017-01-01 00:00:00', 1), \
('Static-Radio-883', 'At Static Radio, we drown out the noise', 'http://sc1.christiannetcast.com:9004', 'http://www.mystaticradio.com', 'Static-Radio-883.png', 0, 0, 0, 1, '2011-11-11', 10342, 0, '2017-01-01 00:00:00', 1), \
('WMBG-740', '', 'http://stream.wmbgradio.com:8000/wmbg-high-quality.ogg', 'http://www.wmbgradio.com/', 'WMBG-740.png', 0, 0, 0, 1, '2011-11-11', 10345, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql31="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('WJEJ-1240', 'Classic Radio - Hometown radio for the entire family...Fantastic light and easy music from yesterday and today.  LIVE, Local DJ&#39;s, Local News and Weather, Local Sports, Talk Shows, Big Band Jump, ', 'http://www.ophanim.net:7900', 'http://www.wjejradio.com/', 'WJEJ-1240.png', 0, 0, 0, 1, '2011-11-11', 10368, 0, '2017-01-01 00:00:00', 1), \
('Radio-Ballerup-902', '', 'http://96.31.83.86:8200/', 'http://www.radioballerup.com/', 'Radio-Ballerup-902.png', 0, 0, 0, 1, '2011-11-11', 10461, 0, '2017-01-01 00:00:00', 1);";

const char *radio_station_setupsql34="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR P4 Esbjerg', '', 'http://live-icy.gss.dr.dk:8000/A/A15H.mp3', 'http://www.dr.dk/Regioner/Esbjerg/', 'DR-P4-Esbjerg-990.png', 0, 0, 0, 1, '2011-11-11', 11233, 0, '2017-01-01 00:00:00', 1), \
('DR Nyheder', '', 'http://live-icy.gss.dr.dk:8000/A/A02L.mp3', 'http://www.dr.dk/radio/alle_kanaler/nyheder.asp', 'DR-Nyheder.png', 0, 0, 0, 1, '2011-11-11', 11396, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Trekanten', '', 'http://live-icy.gss.dr.dk:8000/A/A13H.mp3', 'http://www.dr.dk/p4/trekanten/', 'DR-P4-Trekanten-940.png', 0, 0, 0, 1, '2011-11-11', 11450, 0, '2017-01-01 00:00:00', 1);";


// from here check radio stations






const char *radio_station_setupsql35="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Impact-89-FM-889', '', 'http://96.31.83.86:8200/', 'http://impact89fm.org/', 'Impact-89-FM-889.png', 0, 0, 0, 1, '2011-11-11', 11602, 0, '2017-01-01 00:00:00', 0), \
('New-York-Jets-Pre-Game-Show', '', 'http://96.31.83.86:8200/', '', 'New-York-Jets-Pre-Game-Show.png', 0, 0, 0, 1, '2011-11-11', 11603, 0, '2017-01-01 00:00:00', 0), \
('Atlantic-Tunnel', '', 'http://96.31.83.86:8200/', '', 'Atlantic-Tunnel.png', 0, 0, 0, 1, '2011-11-11', 11604, 0, '2017-01-01 00:00:00', 0), \
('Nachtmahr', '', 'http://96.31.83.86:8200/', '', 'Nachtmahr.png', 0, 0, 0, 1, '2011-11-11', 11605, 0, '2017-01-01 00:00:00', 0), \
('Bone-Thugs-n-Harmony', '', 'http://96.31.83.86:8200/', '', 'Bone-Thugs-n-Harmony.png', 0, 0, 0, 1, '2011-11-11', 11606, 0, '2017-01-01 00:00:00', 0), \
('GotRadio-Rockin-80s', 'Hair Bands of the 80. Great White, Kiss, Iron Maiden, Whitesnake, Poison, Def Leppard, Scorpions, and Motley Crue.', 'http://96.31.83.86:8200/', 'http://www.gotradio.com/', 'GotRadio-Rockin-80s.png', 0, 0, 0, 1, '2011-11-11', 11607, 0, '2017-01-01 00:00:00', 0), \
('Sly-in-the-Morning', '', 'http://96.31.83.86:8200/', '', 'Sly-in-the-Morning.png', 0, 0, 0, 1, '2011-11-11', 11608, 0, '2017-01-01 00:00:00', 0), \
('WZID-957', 'WZID (95.7 FM) is an American radio station located at 500 Commercial Street in Manchester, New Hampshire. In July 1971, WKBR FM became WZID FM, and the format was flipped to beautiful music / AC.', 'http://96.31.83.86:8200/', 'http://www.wzid.com/', 'WZID-957.png', 0, 0, 0, 1, '2011-11-11', 11609, 0, '2017-01-01 00:00:00', 0), \
('Cool-Oldies-1340', '', 'http://96.31.83.86:8200/', 'http://www.1340wraw.com/', 'Cool-Oldies-1340.png', 0, 0, 0, 1, '2011-11-11', 11610, 0, '2017-01-01 00:00:00', 0), \
('Diddy---Dirty-Money', '', 'http://96.31.83.86:8200/', '', 'Diddy---Dirty-Money.png', 0, 0, 0, 1, '2011-11-11', 11611, 0, '2017-01-01 00:00:00', 0), \
('Focus-On-The-Family-Weekend', '', 'http://96.31.83.86:8200/', '', 'Focus-On-The-Family-Weekend.png', 0, 0, 0, 1, '2011-11-11', 11612, 0, '2017-01-01 00:00:00', 0), \
('No-Holds-Barred', '', 'http://96.31.83.86:8200/', '', 'No-Holds-Barred.png', 0, 0, 0, 1, '2011-11-11', 11613, 0, '2017-01-01 00:00:00', 0), \
('HOT-103-JAMZ-1033', '', 'http://96.31.83.86:8200/', 'http://www.kprs.com/', 'HOT-103-JAMZ-1033.png', 0, 0, 0, 1, '2011-11-11', 11614, 0, '2017-01-01 00:00:00', 0), \
('Music-Plus-Zen', '', 'http://96.31.83.86:8200/', 'http://www.musicpluslaradio.com/', 'Music-Plus-Zen.png', 0, 0, 0, 1, '2011-11-11', 11615, 0, '2017-01-01 00:00:00', 0), \
('House-of-Hair', '', 'http://96.31.83.86:8200/', '', 'House-of-Hair.png', 0, 0, 0, 1, '2011-11-11', 11616, 0, '2017-01-01 00:00:00', 0), \
('Warner-Pacific-Knights-Womens-Basketball', 'Listen live to Warner Pacific College Lady Knights basketball with &#39;The Voice of Your Knights&#39;, Jeremy Scott!<BR><BR>&quot;Running with the Pac&quot; begins ten minutes prior to tip-off, featu', 'http://96.31.83.86:8200/', 'http://www.wpcknights.com', 'Warner-Pacific-Knights-Womens-Basketball.png', 0, 0, 0, 1, '2011-11-11', 11617, 0, '2017-01-01 00:00:00', 0), \
('Answers-With-Ken-Ham', '', 'http://96.31.83.86:8200/', '', 'Answers-With-Ken-Ham.png', 0, 0, 0, 1, '2011-11-11', 11618, 0, '2017-01-01 00:00:00', 0), \
('Radio-Luxembourg', 'This is Radio Luxembourg! The Legend Is Back! The Best In Classic Rock!', 'http://96.31.83.86:8200/', 'http://www.radioluxembourg.co.uk/', 'Radio-Luxembourg.png', 0, 0, 0, 1, '2011-11-11', 11619, 0, '2017-01-01 00:00:00', 0), \
('BBC-Radio-1s-Teen-Awards', '', 'http://96.31.83.86:8200/', '', 'BBC-Radio-1s-Teen-Awards.png', 0, 0, 0, 1, '2011-11-11', 11620, 0, '2017-01-01 00:00:00', 0), \
('Findlay-vs-Whitmer-Oct-7-2011', '', 'http://96.31.83.86:8200/', '', 'Findlay-vs-Whitmer-Oct-7-2011.png', 0, 0, 0, 1, '2011-11-11', 11621, 0, '2017-01-01 00:00:00', 0), \
('Nick-and-Nick', '', 'http://96.31.83.86:8200/', '', 'Nick-and-Nick.png', 0, 0, 0, 1, '2011-11-11', 11622, 0, '2017-01-01 00:00:00', 0), \
('BBC-World-Service-News', '', 'http://96.31.83.86:8200/', 'http://www.bbc.co.uk/worldservice/', 'BBC-World-Service-News.png', 0, 0, 0, 1, '2011-11-11', 11624, 0, '2017-01-01 00:00:00', 0), \
('Jack-FM-1011', '', 'http://96.31.83.86:8200/', 'http://jack1011.com', 'Jack-FM-1011.png', 0, 0, 0, 1, '2011-11-11', 11625, 0, '2017-01-01 00:00:00', 0), \
('The-Sheila-Divine', '', 'http://96.31.83.86:8200/', '', 'The-Sheila-Divine.png', 0, 0, 0, 1, '2011-11-11', 11626, 0, '2017-01-01 00:00:00', 0), \
('Jack-Fm-1007', '', 'http://96.31.83.86:8200/', 'http://www.sandiegojack.com/', 'Jack-Fm-1007.png', 0, 0, 0, 1, '2011-11-11', 11627, 0, '2017-01-01 00:00:00', 0), \
('Stan-Walker', '', 'http://96.31.83.86:8200/', '', 'Stan-Walker.png', 0, 0, 0, 1, '2011-11-11', 11628, 0, '2017-01-01 00:00:00', 0), \
('Mavado', '', 'http://96.31.83.86:8200/', '', 'Mavado.png', 0, 0, 0, 1, '2011-11-11', 11629, 0, '2017-01-01 00:00:00', 0), \
('The-Grascals', '', 'http://96.31.83.86:8200/', '', 'The-Grascals.png', 0, 0, 0, 1, '2011-11-11', 11630, 0, '2017-01-01 00:00:00', 0), \
('Meshuggah', '', 'http://96.31.83.86:8200/', '', 'Meshuggah.png', 0, 0, 0, 1, '2011-11-11', 11631, 0, '2017-01-01 00:00:00', 0), \
('Michael-McDonald', '', 'http://96.31.83.86:8200/', '', 'Michael-McDonald.png', 0, 0, 0, 1, '2011-11-11', 11632, 0, '2017-01-01 00:00:00', 0), \
('KCLW-900', '', 'http://96.31.83.86:8200/', 'http://www.kclw.com/', 'KCLW-900.png', 0, 0, 0, 1, '2011-11-11', 11633, 0, '2017-01-01 00:00:00', 0), \
('The-Music-of-the-Stars', '', 'http://96.31.83.86:8200/', '', 'The-Music-of-the-Stars.png', 0, 0, 0, 1, '2011-11-11', 11634, 0, '2017-01-01 00:00:00', 0), \
('Desert-Island-Jazz', '', 'http://96.31.83.86:8200/', '', 'Desert-Island-Jazz.png', 0, 0, 0, 1, '2011-11-11', 11635, 0, '2017-01-01 00:00:00', 0), \
('KRJB-1065', '', 'http://96.31.83.86:8200/', 'http://www.krjbradio.com/', 'KRJB-1065.png', 0, 0, 0, 1, '2011-11-11', 11636, 0, '2017-01-01 00:00:00', 0), \
('Country-1075', '', 'http://96.31.83.86:8200/', 'http://www.koltcountry.com/', 'Country-1075.png', 0, 0, 0, 1, '2011-11-11', 11637, 0, '2017-01-01 00:00:00', 0), \
('WJOX-Gameday', '', 'http://96.31.83.86:8200/', '', 'WJOX-Gameday.png', 0, 0, 0, 1, '2011-11-11', 11638, 0, '2017-01-01 00:00:00', 0), \
('Call-Cundy', '', 'http://96.31.83.86:8200/', '', 'Call-Cundy.png', 0, 0, 0, 1, '2011-11-11', 11639, 0, '2017-01-01 00:00:00', 0), \
('KXLQ-1490', '', 'http://96.31.83.86:8200/', '', 'KXLQ-1490.png', 0, 0, 0, 1, '2011-11-11', 11640, 0, '2017-01-01 00:00:00', 0), \
('The-Jordan-Breen-Show', '', 'http://96.31.83.86:8200/', '', 'The-Jordan-Breen-Show.png', 0, 0, 0, 1, '2011-11-11', 11641, 0, '2017-01-01 00:00:00', 0), \
('The-Jock-1150', '', 'http://96.31.83.86:8200/', 'http://thejock1150.com/', 'The-Jock-1150.png', 0, 0, 0, 1, '2011-11-11', 11642, 0, '2017-01-01 00:00:00', 0), \
('Schlagerlawine24-Radio', '', 'http://96.31.83.86:8200/', 'http://www.schlagerlawine24.de/', 'Schlagerlawine24-Radio.png', 0, 0, 0, 1, '2011-11-11', 11643, 0, '2017-01-01 00:00:00', 0), \
('KSAR-923', '', 'http://96.31.83.86:8200/', '', 'KSAR-923.png', 0, 0, 0, 1, '2011-11-11', 11644, 0, '2017-01-01 00:00:00', 0), \
('WLBE-790', '', 'http://96.31.83.86:8200/', 'http://www.cflradio.net', 'WLBE-790.png', 0, 0, 0, 1, '2011-11-11', 11645, 0, '2017-01-01 00:00:00', 0), \
('Pray-for-the-Nations', '', 'http://96.31.83.86:8200/', '', 'Pray-for-the-Nations.png', 0, 0, 0, 1, '2011-11-11', 11646, 0, '2017-01-01 00:00:00', 0), \
('ESPNEWS', '', 'http://96.31.83.86:8200/', 'http://www.espnews.com/', 'ESPNEWS.png', 0, 0, 0, 1, '2011-11-11', 11647, 0, '2017-01-01 00:00:00', 0), \
('NOS-Langs-de-lijn', '', 'http://96.31.83.86:8200/', '', 'NOS-Langs-de-lijn.png', 0, 0, 0, 1, '2011-11-11', 11648, 0, '2017-01-01 00:00:00', 0), \
('WHLO-640', '', 'http://96.31.83.86:8200/', 'http://www.640whlo.com/', 'WHLO-640.png', 0, 0, 0, 1, '2011-11-11', 11649, 0, '2017-01-01 00:00:00', 0), \
('Grace-Lutheran-Church', '', 'http://96.31.83.86:8200/', '', 'Grace-Lutheran-Church.png', 0, 0, 0, 1, '2011-11-11', 11650, 0, '2017-01-01 00:00:00', 0), \
('WMFR-1230', '', 'http://96.31.83.86:8200/', 'http://www.triadsports.com/', 'WMFR-1230.png', 0, 0, 0, 1, '2011-11-11', 11651, 0, '2017-01-01 00:00:00', 0), \
('Dynamo-All-Access', '', 'http://96.31.83.86:8200/', '', 'Dynamo-All-Access.png', 0, 0, 0, 1, '2011-11-11', 11652, 0, '2017-01-01 00:00:00', 0), \
('SportsJuice---Dayton-Gems', '', 'http://96.31.83.86:8200/', 'http://sportsjuice.com/', 'SportsJuice---Dayton-Gems.png', 0, 0, 0, 1, '2011-11-11', 11653, 0, '2017-01-01 00:00:00', 0), \
('DR-2', '', 'http://96.31.83.86:8200/', 'http://www.dr.dk/', 'DR-2.png', 0, 0, 1, 1, '2011-11-11', 11654, 0, '2011-10-14 16:08:04', 0), \
('WNYC-FM-939', '', 'http://96.31.83.86:8200/', 'http://www.wnyc.org/', 'WNYC-FM-939.png', 0, 0, 0, 1, '2011-11-11', 11655, 0, '2017-01-01 00:00:00', 0), \
('Inside-with-Saints-Hall-Of-Fame', '', 'http://96.31.83.86:8200/', '', 'Inside-with-Saints-Hall-Of-Fame.png', 0, 0, 0, 1, '2011-11-11', 11656, 0, '2017-01-01 00:00:00', 0), \
('Bob-FM-1039', '', 'http://96.31.83.86:8200/', 'http://www.bobfm1039.com/', 'Bob-FM-1039.png', 0, 0, 0, 1, '2011-11-11', 11657, 0, '2017-01-01 00:00:00', 0), \
('SportsRadio-610', 'SportsRadio 610 is Houston&#39;s #1 station for sports talk and more.   Listeners get to talk sports with an all-star line up including Marc Vandermeer, John Lopez, Josh Innes, Jim Rome, Rich Lord, Ro', 'http://96.31.83.86:8200/', 'http://www.sportsradio610.com/', 'SportsRadio-610.png', 0, 0, 0, 1, '2011-11-11', 11658, 0, '2017-01-01 00:00:00', 0), \
('Rick-Allen', '', 'http://96.31.83.86:8200/', '', 'Rick-Allen.png', 0, 0, 0, 1, '2011-11-11', 11659, 0, '2017-01-01 00:00:00', 0), \
('Building-429', '', 'http://96.31.83.86:8200/', '', 'Building-429.png', 0, 0, 0, 1, '2011-11-11', 11660, 0, '2017-01-01 00:00:00', 0), \
('WMT-600', '', 'http://96.31.83.86:8200/', 'http://www.wmtradio.com/', 'WMT-600.png', 0, 0, 0, 1, '2011-11-11', 11661, 0, '2017-01-01 00:00:00', 0), \
('Jerry-Smith', '', 'http://96.31.83.86:8200/', '', 'Jerry-Smith.png', 0, 0, 0, 1, '2011-11-11', 11662, 0, '2017-01-01 00:00:00', 0), \
('The-Bible-Study-Hour', '', 'http://96.31.83.86:8200/', '', 'The-Bible-Study-Hour.png', 0, 0, 0, 1, '2011-11-11', 11663, 0, '2017-01-01 00:00:00', 0), \
('Radio-del-Amor', 'Radio del Amor comparte contigo lo mejor de la m&#250;sica rom&#225;ntica, hablamos de amor con la m&#250;sica que tan solo al escucharla nos hace sentir sensaciones y emociones que marcaron un moment', 'http://96.31.83.86:8200/', 'http://www.radiodelamor.comlu.com', 'Radio-del-Amor.png', 0, 0, 0, 1, '2011-11-11', 11664, 0, '2017-01-01 00:00:00', 0), \
('Britney-Spears', '', 'http://96.31.83.86:8200/', '', 'Britney-Spears.png', 0, 0, 0, 1, '2011-11-11', 11665, 0, '2017-01-01 00:00:00', 0), \
('CBSSportscom-Fantasy-Baseball-Podcast', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 11666, 0, '2017-01-01 00:00:00', 0), \
('Classic--Contemporary-Gospel-Music', '', 'http://96.31.83.86:8200/', '', 'Classic--Contemporary-Gospel-Music.png', 0, 0, 0, 1, '2011-11-11', 11667, 0, '2017-01-01 00:00:00', 0), \
('Indiana-Hoosiers-at-Wisconsin-Badgers-Oct-15-2011', '', 'http://96.31.83.86:8200/', '', 'Indiana-Hoosiers-at-Wisconsin-Badgers-Oct-15-2011.png', 0, 0, 0, 1, '2011-11-11', 11668, 0, '2017-01-01 00:00:00', 0), \
('Sportstalk-(WNML)', '', 'http://96.31.83.86:8200/', '', 'Sportstalk-(WNML).png', 0, 0, 0, 1, '2011-11-11', 11669, 0, '2017-01-01 00:00:00', 0), \
('Ann-Peebles', '', 'http://96.31.83.86:8200/', '', 'Ann-Peebles.png', 0, 0, 0, 1, '2011-11-11', 11670, 0, '2017-01-01 00:00:00', 0), \
('WEBE-108-1079', '', 'http://96.31.83.86:8200/', 'http://www.webe108.com/', 'WEBE-108-1079.png', 0, 0, 0, 1, '2011-11-11', 11671, 0, '2017-01-01 00:00:00', 0), \
('Kuschel-FM', 'Da schmilzt die Butter im Kühlschrank.', 'http://96.31.83.86:8200/', 'http://www.kuschel.fm/', 'Kuschel-FM.png', 0, 0, 0, 1, '2011-11-11', 11672, 0, '2017-01-01 00:00:00', 0), \
('TechStuff', '', 'http://96.31.83.86:8200/', '', 'TechStuff.png', 0, 0, 0, 1, '2011-11-11', 11673, 0, '2017-01-01 00:00:00', 0), \
('WFOS-887', 'WFOS features Blues, Beach, Oldies, R&amp;B and a wide variety of other music that other stations don&#39;t play, as well as national and local news.', 'http://96.31.83.86:8200/', 'http://www.cpschools.com/departments/radio/', 'WFOS-887.png', 0, 0, 0, 1, '2011-11-11', 11674, 0, '2017-01-01 00:00:00', 0), \
('WFUN-970', '', 'http://96.31.83.86:8200/', 'http://www.wfunam97.com/', 'WFUN-970.png', 0, 0, 0, 1, '2011-11-11', 11675, 0, '2017-01-01 00:00:00', 0), \
('Nutty-News', '', 'http://96.31.83.86:8200/', '', 'Nutty-News.png', 0, 0, 0, 1, '2011-11-11', 11676, 0, '2017-01-01 00:00:00', 0), \
('WSLA-1560', 'WSLA is an ESPN Sports Talk affiliate broadcasting from New Orleans on 1560 AM and online.', 'http://96.31.83.86:8200/', 'http://www.wslaradio.com', 'WSLA-1560.png', 0, 0, 0, 1, '2011-11-11', 11677, 0, '2017-01-01 00:00:00', 0), \
('In-the-Zone', '', 'http://96.31.83.86:8200/', '', 'In-the-Zone.png', 0, 0, 0, 1, '2011-11-11', 11678, 0, '2017-01-01 00:00:00', 0), \
('Kick-FM-969', 'kick!fm ist ein freies, nicht-kommerzielles H&#246;rfunkprogramm, das sich auf Dance, Uptempo und Electro House spezialisiert und sich vornehmlich an die Zielgruppe zwischen 14 und 29 Jahren richtet. ', 'http://96.31.83.86:8200/', 'http://www.kickfm.eu/', 'Kick-FM-969.png', 0, 0, 0, 1, '2011-11-11', 11679, 0, '2017-01-01 00:00:00', 0), \
('ENERGY-Music-Show', '', 'http://96.31.83.86:8200/', '', 'ENERGY-Music-Show.png', 0, 0, 0, 1, '2011-11-11', 11680, 0, '2017-01-01 00:00:00', 0), \
('WPHX-1220', '', 'http://96.31.83.86:8200/', '', 'WPHX-1220.png', 0, 0, 0, 1, '2011-11-11', 11681, 0, '2017-01-01 00:00:00', 0), \
('Prime-Time-Sports-Interactive', '', 'http://96.31.83.86:8200/', '', 'Prime-Time-Sports-Interactive.png', 0, 0, 0, 1, '2011-11-11', 11682, 0, '2017-01-01 00:00:00', 0), \
('Dave-Rothenberg', '', 'http://96.31.83.86:8200/', '', 'Dave-Rothenberg.png', 0, 0, 0, 1, '2011-11-11', 11683, 0, '2017-01-01 00:00:00', 0), \
('WXBR-1460', '', 'http://96.31.83.86:8200/', 'http://1460wxbr.com/', 'WXBR-1460.png', 0, 0, 0, 1, '2011-11-11', 11684, 0, '2017-01-01 00:00:00', 0), \
('Frog-Country-923', '', 'http://96.31.83.86:8200/', 'http://www.frogcountry923.com/', 'Frog-Country-923.png', 0, 0, 0, 1, '2011-11-11', 11685, 0, '2017-01-01 00:00:00', 0), \
('1550-ESPN', '', 'http://96.31.83.86:8200/', 'http://www.1550espn.com/', '1550-ESPN.png', 0, 0, 0, 1, '2011-11-11', 11686, 0, '2017-01-01 00:00:00', 0), \
('Mix-58-FM', '', 'http://96.31.83.86:8200/', 'http://www.mix58fm.com/', 'Mix-58-FM.png', 0, 0, 0, 1, '2011-11-11', 11687, 0, '2017-01-01 00:00:00', 0), \
('The-River-1063', '', 'http://96.31.83.86:8200/', 'http://www.1063theriver.com/', 'The-River-1063.png', 0, 0, 0, 1, '2011-11-11', 11688, 0, '2017-01-01 00:00:00', 0), \
('Pop-FM-1000', '', 'http://96.31.83.86:8200/', 'http://www.popfm.dk/', 'Pop-FM-1000.png', 0, 0, 0, 1, '2011-11-11', 11689, 0, '2017-01-01 00:00:00', 0), \
('Dinner-Jazz', '', 'http://96.31.83.86:8200/', '', 'Dinner-Jazz.png', 0, 0, 0, 1, '2011-11-11', 11690, 0, '2017-01-01 00:00:00', 0), \
('KXT-917', 'KXT is a new radio station found at 91.7 FM in North Texas, and at kxt.org worldwide. Alt-country, indie rock, alternative and world music', 'http://96.31.83.86:8200/', 'http://www.kxt.org/', 'KXT-917.png', 0, 0, 0, 1, '2011-11-11', 11691, 0, '2017-01-01 00:00:00', 0), \
('NRJ-Soundtrack', '', 'http://96.31.83.86:8200/', 'http://www.energyzueri.ch/', 'NRJ-Soundtrack.png', 0, 0, 0, 1, '2011-11-11', 11692, 0, '2017-01-01 00:00:00', 0), \
('Die', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 11693, 0, '2017-01-01 00:00:00', 0), \
('FunX-Slow-Jamz', '', 'http://96.31.83.86:8200/', 'http://www.funx.nl/', 'FunX-Slow-Jamz.png', 0, 0, 0, 1, '2011-11-11', 11694, 0, '2017-01-01 00:00:00', 0), \
('A-Touch-of-Grey', '', 'http://96.31.83.86:8200/', '', 'A-Touch-of-Grey.png', 0, 0, 0, 1, '2011-11-11', 11695, 0, '2017-01-01 00:00:00', 0), \
('Ross-Parsley', '', 'http://96.31.83.86:8200/', '', 'Ross-Parsley.png', 0, 0, 0, 1, '2011-11-11', 11696, 0, '2017-01-01 00:00:00', 0), \
('Compass Media Oakland Raiders Network', 'Compass Media Networks is the exclusive home for the Oakland Raiders.', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 11697, 0, '2017-01-01 00:00:00', 0), \
('Brooks--Dunn', '', 'http://96.31.83.86:8200/', '', 'Brooks--Dunn.png', 0, 0, 0, 1, '2011-11-11', 11698, 0, '2017-01-01 00:00:00', 0), \
('The-SCORE-1210', '', 'http://96.31.83.86:8200/', 'http://www.thescore1210.com/', 'The-SCORE-1210.png', 0, 0, 0, 1, '2011-11-11', 11699, 0, '2017-01-01 00:00:00', 0), \
('Hot-Chocolate', '', 'http://96.31.83.86:8200/', '', 'Hot-Chocolate.png', 0, 0, 0, 1, '2011-11-11', 11700, 0, '2017-01-01 00:00:00', 0), \
('Radio-Subasio-887', 'TAGLIO HIT RADIO DEDICATA AD UN PUBBLICO ADULTO CON NOVITA&#39; ITALIANE E STRANIERE E GRANDI CLASSICI ANCHE RICERCATI DEL PASSATO.', 'http://96.31.83.86:8200/', 'http://www.radiosubasio.it/', 'Radio-Subasio-887.png', 0, 0, 0, 1, '2011-11-11', 11701, 0, '2017-01-01 00:00:00', 0), \
('Denis-Prior', '', 'http://96.31.83.86:8200/', '', 'Denis-Prior.png', 0, 0, 0, 1, '2011-11-11', 11702, 0, '2017-01-01 00:00:00', 0), \
('LatinFM---Baladas', '', 'http://96.31.83.86:8200/', 'http://www.latin.fm/', 'LatinFM---Baladas.png', 0, 0, 0, 1, '2011-11-11', 11703, 0, '2017-01-01 00:00:00', 0), \
('Just-Classics', '', 'http://96.31.83.86:8200/', '', 'Just-Classics.png', 0, 0, 0, 1, '2011-11-11', 11704, 0, '2017-01-01 00:00:00', 0), \
('Afterlife', '', 'http://96.31.83.86:8200/', '', 'Afterlife.png', 0, 0, 0, 1, '2011-11-11', 11705, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-710', '', 'http://96.31.83.86:8200/', 'http://www.espn710am.com/', 'ESPN-Radio-710.png', 0, 0, 0, 1, '2011-11-11', 11706, 0, '2017-01-01 00:00:00', 0), \
('Travis-Tritt', '', 'http://96.31.83.86:8200/', '', 'Travis-Tritt.png', 0, 0, 0, 1, '2011-11-11', 11707, 0, '2017-01-01 00:00:00', 0), \
('Heart-1049-FM', '', 'http://96.31.83.86:8200/', 'http://www.1049.fm/', 'Heart-1049-FM.png', 0, 0, 0, 1, '2011-11-11', 11708, 0, '2017-01-01 00:00:00', 0), \
('NFL-Now', '', 'http://96.31.83.86:8200/', '', 'NFL-Now.png', 0, 0, 0, 1, '2011-11-11', 11709, 0, '2017-01-01 00:00:00', 0), \
('Star-1400', '', 'http://96.31.83.86:8200/', 'http://www.star1400.com/', 'Star-1400.png', 0, 0, 0, 1, '2011-11-11', 11710, 0, '2017-01-01 00:00:00', 0), \
('Jerry-Butler', '', 'http://96.31.83.86:8200/', '', 'Jerry-Butler.png', 0, 0, 0, 1, '2011-11-11', 11711, 0, '2017-01-01 00:00:00', 0), \
('Sean-Kingston', '', 'http://96.31.83.86:8200/', '', 'Sean-Kingston.png', 0, 0, 0, 1, '2011-11-11', 11712, 0, '2017-01-01 00:00:00', 0), \
('SportsJuice---New-England-College', '', 'http://96.31.83.86:8200/', 'http://sportsjuice.com/', 'SportsJuice---New-England-College.png', 0, 0, 0, 1, '2011-11-11', 11713, 0, '2017-01-01 00:00:00', 0), \
('Dee-C-Lee', '', 'http://96.31.83.86:8200/', '', 'Dee-C-Lee.png', 0, 0, 0, 1, '2011-11-11', 11714, 0, '2017-01-01 00:00:00', 0), \
('KGAS-1590', '', 'http://96.31.83.86:8200/', 'http://www.kgasradio.com/', 'KGAS-1590.png', 0, 0, 0, 1, '2011-11-11', 11715, 0, '2017-01-01 00:00:00', 0), \
('The-Manhattans', '', 'http://96.31.83.86:8200/', '', 'The-Manhattans.png', 0, 0, 0, 1, '2011-11-11', 11716, 0, '2017-01-01 00:00:00', 0), \
('Rob-da-Bank', '', 'http://96.31.83.86:8200/', '', 'Rob-da-Bank.png', 0, 0, 0, 1, '2011-11-11', 11717, 0, '2017-01-01 00:00:00', 0), \
('Sky-Sports-Radio-1017', '', 'http://96.31.83.86:8200/', 'http://www.skysportsradio.com.au/', 'Sky-Sports-Radio-1017.png', 0, 0, 0, 1, '2011-11-11', 11718, 0, '2017-01-01 00:00:00', 0), \
('Warrant', '', 'http://96.31.83.86:8200/', '', 'Warrant.png', 0, 0, 0, 1, '2011-11-11', 11719, 0, '2017-01-01 00:00:00', 0), \
('Q-979-WJBQ', '', 'http://96.31.83.86:8200/', 'http://www.wjbq.com/', 'Q-979-WJBQ.png', 0, 0, 0, 1, '2011-11-11', 11720, 0, '2017-01-01 00:00:00', 0), \
('KRTZ-987', 'KRTZ is the radio leader in local news, weather, sports and other information...along with the best music on the radio.<BR><BR>Our objective is to be the leading radio broadcast station in the region,', 'http://96.31.83.86:8200/', 'http://www.krtzradio.com/', 'KRTZ-987.png', 0, 0, 0, 1, '2011-11-11', 11721, 0, '2017-01-01 00:00:00', 0), \
('Costa-Verde-FM-917', '', 'http://96.31.83.86:8200/', 'http://www.radiocostaverdefm.com.br/', 'Costa-Verde-FM-917.png', 0, 0, 0, 1, '2011-11-11', 11722, 0, '2017-01-01 00:00:00', 0), \
('Country-Action', '', 'http://96.31.83.86:8200/', '', 'Country-Action.png', 0, 0, 0, 1, '2011-11-11', 11723, 0, '2017-01-01 00:00:00', 0), \
('Radio-Polskie---Club-Hits', '', 'http://96.31.83.86:8200/', 'http://www.polskastacja.pl/', 'Radio-Polskie---Club-Hits.png', 0, 0, 0, 1, '2011-11-11', 11724, 0, '2017-01-01 00:00:00', 0), \
('Paul-Gambaccini', '', 'http://96.31.83.86:8200/', '', 'Paul-Gambaccini.png', 0, 0, 0, 1, '2011-11-11', 11725, 0, '2017-01-01 00:00:00', 0), \
('Thoroughbred-LA', '', 'http://96.31.83.86:8200/', '', 'Thoroughbred-LA.png', 0, 0, 0, 1, '2011-11-11', 11726, 0, '2017-01-01 00:00:00', 0), \
('Jazz-at-Lincoln-Center', '', 'http://96.31.83.86:8200/', '', 'Jazz-at-Lincoln-Center.png', 0, 0, 0, 1, '2011-11-11', 11727, 0, '2017-01-01 00:00:00', 0), \
('Kate-Ryan', '', 'http://96.31.83.86:8200/', '', 'Kate-Ryan.png', 0, 0, 0, 1, '2011-11-11', 11728, 0, '2017-01-01 00:00:00', 0), \
('KGIW-1450', '', 'http://96.31.83.86:8200/', 'http://kgiwkalq.com/', 'KGIW-1450.png', 0, 0, 0, 1, '2011-11-11', 11729, 0, '2017-01-01 00:00:00', 0), \
('Radio-Poniente-945', '', 'http://96.31.83.86:8200/', 'http://www.radioponiente.com/', 'Radio-Poniente-945.png', 0, 0, 0, 1, '2011-11-11', 11730, 0, '2017-01-01 00:00:00', 0), \
('Radio-Euro-Phoria', '', 'http://96.31.83.86:8200/', 'http://RadioEuroPhoria.com', 'Radio-Euro-Phoria.png', 0, 0, 0, 1, '2011-11-11', 11731, 0, '2017-01-01 00:00:00', 0), \
('The-Corner-1061', 'WCNR is an Adult Album Alternative formatted broadcast radio station licensed to Keswick, Virginia, serving the City of Charlottesville and Albemarle and Western Fluvanna Counties.', 'http://96.31.83.86:8200/', 'http://www.1061thecorner.com/', 'The-Corner-1061.png', 0, 0, 0, 1, '2011-11-11', 11732, 0, '2017-01-01 00:00:00', 0), \
('McCoy-Tyner', '', 'http://96.31.83.86:8200/', '', 'McCoy-Tyner.png', 0, 0, 0, 1, '2011-11-11', 11733, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1230', '', 'http://96.31.83.86:8200/', '', 'ESPN-1230.png', 0, 0, 0, 1, '2011-11-11', 11734, 0, '2017-01-01 00:00:00', 0), \
('Sports-Talk-570', '', 'http://96.31.83.86:8200/', 'http://www.sportstalk570.com/', 'Sports-Talk-570.png', 0, 0, 0, 1, '2011-11-11', 11735, 0, '2017-01-01 00:00:00', 0), \
('KOPB-FM-915', '', 'http://96.31.83.86:8200/', 'http://www.opb.org/radio/', 'KOPB-FM-915.png', 0, 0, 0, 1, '2011-11-11', 11736, 0, '2017-01-01 00:00:00', 0), \
('Eric-Marienthal', '', 'http://96.31.83.86:8200/', '', 'Eric-Marienthal.png', 0, 0, 0, 1, '2011-11-11', 11737, 0, '2017-01-01 00:00:00', 0), \
('Stephen-Malkmus-and-the-Jicks', '', 'http://96.31.83.86:8200/', '', 'Stephen-Malkmus-and-the-Jicks.png', 0, 0, 0, 1, '2011-11-11', 11738, 0, '2017-01-01 00:00:00', 0), \
('WINE-940', '', 'http://96.31.83.86:8200/', 'http://www.i95rock.com/', 'WINE-940.png', 0, 0, 0, 1, '2011-11-11', 11741, 0, '2017-01-01 00:00:00', 0), \
('Martin-Solveig', '', 'http://96.31.83.86:8200/', '', 'Martin-Solveig.png', 0, 0, 0, 1, '2011-11-11', 11742, 0, '2017-01-01 00:00:00', 0), \
('KATX-977', 'Small market radio done right! Live morning show, call in show to buy, sell and trade and coverage of community events, high school and college sports and more.', 'http://96.31.83.86:8200/', 'http://www.hprnetwork.com/index_central.html', 'KATX-977.png', 0, 0, 0, 1, '2011-11-11', 11743, 0, '2017-01-01 00:00:00', 0), \
('WFOB-1430', '', 'http://96.31.83.86:8200/', 'http://www.wfob.com/', 'WFOB-1430.png', 0, 0, 0, 1, '2011-11-11', 11744, 0, '2017-01-01 00:00:00', 0), \
('Y-1063', 'Y106.3 is POP CHR. Sometimes referred to simply as Contemporary Hit Radio.. Our playlist also includes a slant toward dance hits and remixes of popular songs. Our play lists consist of new cutting edg', 'http://96.31.83.86:8200/', 'http://www.y1063i.com/', 'Y-1063.png', 0, 0, 0, 1, '2011-11-11', 11745, 0, '2017-01-01 00:00:00', 0), \
('Road-Rage', '', 'http://96.31.83.86:8200/', '', 'Road-Rage.png', 0, 0, 0, 1, '2011-11-11', 11746, 0, '2017-01-01 00:00:00', 0), \
('Pat-Day', '', 'http://96.31.83.86:8200/', '', 'Pat-Day.png', 0, 0, 0, 1, '2011-11-11', 11747, 0, '2017-01-01 00:00:00', 0), \
('I-Love-Radio', 'I Love Radio ist so interaktiv wie kein anderes deutsches Radio-Angebot. Die User bestimmen per Voting aus einer Auswahl brandneuer Chart-Hits st&#252;ndlich neu ihre Top Five, die immer zur vollen St', 'http://96.31.83.86:8200/', 'http://www.iloveradio.de/', 'I-Love-Radio.png', 0, 0, 0, 1, '2011-11-11', 11748, 0, '2017-01-01 00:00:00', 0), \
('P5-Stavanger-1022', '', 'http://96.31.83.86:8200/', 'http://stavanger.p5.no', 'P5-Stavanger-1022.png', 0, 0, 0, 1, '2011-11-11', 11749, 0, '2017-01-01 00:00:00', 0), \
('SeeJay-Radio', '', 'http://96.31.83.86:8200/', 'http://www.seejay.cz/', 'SeeJay-Radio.png', 0, 0, 0, 1, '2011-11-11', 11750, 0, '2017-01-01 00:00:00', 0), \
('Robbie-Dupree', '', 'http://96.31.83.86:8200/', '', 'Robbie-Dupree.png', 0, 0, 0, 1, '2011-11-11', 11751, 0, '2017-01-01 00:00:00', 0), \
('KFBK-Informative-Programming', '', 'http://96.31.83.86:8200/', '', 'KFBK-Informative-Programming.png', 0, 0, 0, 1, '2011-11-11', 11752, 0, '2017-01-01 00:00:00', 0), \
('Hits-My-Music-Zen', '', 'http://96.31.83.86:8200/', 'http://www.hitsmymusic.com/', 'Hits-My-Music-Zen.png', 0, 0, 0, 1, '2011-11-11', 11753, 0, '2017-01-01 00:00:00', 0), \
('WDTX-1005', '', 'http://96.31.83.86:8200/', 'http://www.espn1005.com/', 'WDTX-1005.png', 0, 0, 0, 1, '2011-11-11', 11754, 0, '2017-01-01 00:00:00', 0), \
('Sebastian-Gamboa', '', 'http://96.31.83.86:8200/', '', 'Sebastian-Gamboa.png', 0, 0, 0, 1, '2011-11-11', 11755, 0, '2017-01-01 00:00:00', 0), \
('Hot-1039', '', 'http://96.31.83.86:8200/', 'http://www.hot1039fm.com/', 'Hot-1039.png', 0, 0, 0, 1, '2011-11-11', 11756, 0, '2017-01-01 00:00:00', 0), \
('KOOL-991', '', 'http://96.31.83.86:8200/', 'http://www.kool991.com/', 'KOOL-991.png', 0, 0, 0, 1, '2011-11-11', 11757, 0, '2017-01-01 00:00:00', 0), \
('Moore-Country-104-1049', '', 'http://96.31.83.86:8200/', 'http://wskvfm.com/', 'Moore-Country-104-1049.png', 0, 0, 0, 1, '2011-11-11', 11758, 0, '2017-01-01 00:00:00', 0), \
('KJBB-891', '', 'http://96.31.83.86:8200/', 'http://www.kjbbfm.com/', 'KJBB-891.png', 0, 0, 0, 1, '2011-11-11', 11759, 0, '2017-01-01 00:00:00', 0), \
('WOMT-1240', '', 'http://96.31.83.86:8200/', 'http://www.womtradio.com/', 'WOMT-1240.png', 0, 0, 0, 1, '2011-11-11', 11760, 0, '2017-01-01 00:00:00', 0), \
('Roy-Ayers', '', 'http://96.31.83.86:8200/', '', 'Roy-Ayers.png', 0, 0, 0, 1, '2011-11-11', 11761, 0, '2017-01-01 00:00:00', 0);";



const char *radio_station_setupsql36="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Today-in-iPhone', '', 'http://96.31.83.86:8200/', '', 'Today-in-iPhone.png', 0, 0, 0, 1, '2011-11-11', 11763, 0, '2017-01-01 00:00:00', 0), \
('The-BEAR-660', '', 'http://96.31.83.86:8200/', 'http://www.660wbhr.com/', 'The-BEAR-660.png', 0, 0, 0, 1, '2011-11-11', 11764, 0, '2017-01-01 00:00:00', 0), \
('Yolanda-Adams', '', 'http://96.31.83.86:8200/', '', 'Yolanda-Adams.png', 0, 0, 0, 1, '2011-11-11', 11765, 0, '2017-01-01 00:00:00', 0), \
('BB-King', '', 'http://96.31.83.86:8200/', '', 'BB-King.png', 0, 0, 1, 1, '2011-11-11', 11766, 0, '2011-10-14 16:03:02', 0), \
('The-Edge-1021', '', 'http://96.31.83.86:8200/', 'http://www.kdge.com/', 'The-Edge-1021.png', 0, 0, 0, 1, '2011-11-11', 11767, 0, '2017-01-01 00:00:00', 0), \
('Radiomiaminet-Miami', 'Radiomiami.net from Los Angeles, Miami and Barcelona. Radiomiami.net coast to coast in USA and around the world by internet.', 'http://96.31.83.86:8200/', 'http://www.radiomiami.net/', 'Radiomiaminet-Miami.png', 0, 0, 0, 1, '2011-11-11', 11768, 0, '2017-01-01 00:00:00', 0), \
('Fareed-Zakaria-GPS', '', 'http://96.31.83.86:8200/', '', 'Fareed-Zakaria-GPS.png', 0, 0, 0, 1, '2011-11-11', 11769, 0, '2017-01-01 00:00:00', 0), \
('WMMN-920', '', 'http://96.31.83.86:8200/', 'http://www.920wmmn.com/', 'WMMN-920.png', 0, 0, 0, 1, '2011-11-11', 11770, 0, '2017-01-01 00:00:00', 0), \
('Radio-Thugzone', 'Thugzone.com Radio is crunkest radio station online, complete with exlusive contests, live shows, up to date release dates and southern news.', 'http://96.31.83.86:8200/', 'http://www.thugzone.com/', 'Radio-Thugzone.png', 0, 0, 0, 1, '2011-11-11', 11771, 0, '2017-01-01 00:00:00', 0), \
('Live-955', '', 'http://96.31.83.86:8200/', 'http://www.live955.com/', 'Live-955.png', 0, 0, 0, 1, '2011-11-11', 11772, 0, '2017-01-01 00:00:00', 0), \
('AM-Outdoors', '', 'http://96.31.83.86:8200/', '', 'AM-Outdoors.png', 0, 0, 1, 1, '2011-11-11', 11773, 0, '2011-10-15 20:12:36', 0), \
('Right-Said-Fred', '', 'http://96.31.83.86:8200/', '', 'Right-Said-Fred.png', 0, 0, 0, 1, '2011-11-11', 11774, 0, '2017-01-01 00:00:00', 0), \
('Nina-Simone', '', 'http://96.31.83.86:8200/', '', 'Nina-Simone.png', 0, 0, 0, 1, '2011-11-11', 11775, 0, '2017-01-01 00:00:00', 0), \
('Alternative-Radio-Coalition', '', 'http://96.31.83.86:8200/', '', 'Alternative-Radio-Coalition.png', 0, 0, 0, 1, '2011-11-11', 11776, 0, '2017-01-01 00:00:00', 0), \
('KJR-950', '', 'http://96.31.83.86:8200/', 'http://www.950kjr.com/', 'KJR-950.png', 0, 0, 0, 1, '2011-11-11', 11777, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-Gospel', '', 'http://96.31.83.86:8200/', '', 'Sunday-Morning-Gospel.png', 0, 0, 0, 1, '2011-11-11', 11778, 0, '2017-01-01 00:00:00', 0), \
('KOMC-FM-1001', '', 'http://96.31.83.86:8200/', 'http://www.komc.com/', 'KOMC-FM-1001.png', 0, 0, 0, 1, '2011-11-11', 11779, 0, '2017-01-01 00:00:00', 0), \
('The-Howard-Simon-Show', '', 'http://96.31.83.86:8200/', '', 'The-Howard-Simon-Show.png', 0, 0, 0, 1, '2011-11-11', 11780, 0, '2017-01-01 00:00:00', 0), \
('WSRT-1067', '', 'http://96.31.83.86:8200/', 'http://www.1067youfm.com', 'WSRT-1067.png', 0, 0, 0, 1, '2011-11-11', 11781, 0, '2017-01-01 00:00:00', 0), \
('La-Gran-D-969', '', 'http://96.31.83.86:8200/', 'http://www.lakebuena.com/', 'La-Gran-D-969.png', 0, 0, 0, 1, '2011-11-11', 11782, 0, '2017-01-01 00:00:00', 0), \
('KCSP-610', '', 'http://96.31.83.86:8200/', 'http://www.610sports.com/', 'KCSP-610.png', 0, 0, 0, 1, '2011-11-11', 11783, 0, '2017-01-01 00:00:00', 0), \
('Sportscentral', '', 'http://96.31.83.86:8200/', '', 'Sportscentral.png', 0, 0, 0, 1, '2011-11-11', 11784, 0, '2017-01-01 00:00:00', 0), \
('Newstalk-1360-K-Five', '', 'http://96.31.83.86:8200/', 'http://www.kfiv1360.com/', 'Newstalk-1360-K-Five.png', 0, 0, 0, 1, '2011-11-11', 11785, 0, '2017-01-01 00:00:00', 0), \
('James-Moody', '', 'http://96.31.83.86:8200/', '', 'James-Moody.png', 0, 0, 0, 1, '2011-11-11', 11786, 0, '2017-01-01 00:00:00', 0), \
('Missed-Fortune-Radio', '', 'http://96.31.83.86:8200/', '', 'Missed-Fortune-Radio.png', 0, 0, 0, 1, '2011-11-11', 11787, 0, '2017-01-01 00:00:00', 0), \
('WAMU-885', '', 'http://96.31.83.86:8200/', 'http://www.wamu.org/', 'WAMU-885.png', 0, 0, 0, 1, '2011-11-11', 11788, 0, '2017-01-01 00:00:00', 0), \
('NDR-2-876', '', 'http://96.31.83.86:8200/', 'http://www.ndr2.de/', 'NDR-2-876.png', 0, 0, 0, 1, '2011-11-11', 11789, 0, '2017-01-01 00:00:00', 0), \
('KINK-1019', 'KINK.FM 101.9 is one of the country best', 'http://96.31.83.86:8200/', 'http://www.kink.fm', 'KINK-1019.png', 0, 0, 0, 1, '2011-11-11', 11790, 0, '2017-01-01 00:00:00', 0), \
('VOA-English-Music-Live', '', 'http://96.31.83.86:8200/', '', 'VOA-English-Music-Live.png', 0, 0, 0, 1, '2011-11-11', 11791, 0, '2017-01-01 00:00:00', 0), \
('Saxophone-Jazz-on-JAZZRADIOcom', 'One of jazzs most popular instruments, from bebop to today.', 'http://96.31.83.86:8200/', 'http://www.jazzradio.com/', 'Saxophone-Jazz-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 11792, 0, '2017-01-01 00:00:00', 0), \
('Hot-1015', '', 'http://96.31.83.86:8200/', 'http://www.hot1015tampabay.com', 'Hot-1015.png', 0, 0, 0, 1, '2011-11-11', 11793, 0, '2017-01-01 00:00:00', 0), \
('Roxette', '', 'http://96.31.83.86:8200/', '', 'Roxette.png', 0, 0, 0, 1, '2011-11-11', 11794, 0, '2017-01-01 00:00:00', 0), \
('First-United-Methodist-Church-Of-Brooksville', '', 'http://96.31.83.86:8200/', '', 'First-United-Methodist-Church-Of-Brooksville.png', 0, 0, 0, 1, '2011-11-11', 11795, 0, '2017-01-01 00:00:00', 0), \
('The-Fish-955', '', 'http://96.31.83.86:8200/', 'http://www.955thefish.com/', 'The-Fish-955.png', 0, 0, 0, 1, '2011-11-11', 11796, 0, '2017-01-01 00:00:00', 0), \
('Northwestern-Outdoors', '', 'http://96.31.83.86:8200/', '', 'Northwestern-Outdoors.png', 0, 0, 0, 1, '2011-11-11', 11797, 0, '2017-01-01 00:00:00', 0), \
('News-Talk-1059', '', 'http://96.31.83.86:8200/', 'http://valdostatoday.com/News.html', 'News-Talk-1059.png', 0, 0, 0, 1, '2011-11-11', 11798, 0, '2017-01-01 00:00:00', 0), \
('Raute-MusikFM-Goldies', '', 'http://96.31.83.86:8200/', 'http://www.rautemusik.fm/', 'Raute-MusikFM-Goldies.png', 0, 0, 0, 1, '2011-11-11', 11799, 0, '2017-01-01 00:00:00', 0), \
('The-Flaming-Lips', '', 'http://96.31.83.86:8200/', '', 'The-Flaming-Lips.png', 0, 0, 0, 1, '2011-11-11', 11800, 0, '2017-01-01 00:00:00', 0), \
('Chennai-FM-Rainbow-1014', '', 'http://96.31.83.86:8200/', 'http://www.tamilradios.com/', 'Chennai-FM-Rainbow-1014.png', 0, 0, 0, 1, '2011-11-11', 11801, 0, '2017-01-01 00:00:00', 0), \
('Craig-Weston', '', 'http://96.31.83.86:8200/', '', 'Craig-Weston.png', 0, 0, 0, 1, '2011-11-11', 11802, 0, '2017-01-01 00:00:00', 0), \
('KMFB-927', 'Eclectic small-town radio. Oldies by day, but a whole &#39;nother thing at night (California time). Local affairs. Rock, Jazz, Blues, Reggae, Rap, Latino, World Music, and unique written-word talk.', 'http://96.31.83.86:8200/', 'http://www.kmfb-fm.com/', 'KMFB-927.png', 0, 0, 0, 1, '2011-11-11', 11803, 0, '2017-01-01 00:00:00', 0), \
('Bushelon-Funeral-Home', '', 'http://96.31.83.86:8200/', '', 'Bushelon-Funeral-Home.png', 0, 0, 0, 1, '2011-11-11', 11804, 0, '2017-01-01 00:00:00', 0), \
('Return-to-God', '', 'http://96.31.83.86:8200/', '', 'Return-to-God.png', 0, 0, 0, 1, '2011-11-11', 11805, 0, '2017-01-01 00:00:00', 0), \
('KISS-FM-1061', '', 'http://96.31.83.86:8200/', 'http://www.kissfmevansville.com/', 'KISS-FM-1061.png', 0, 0, 0, 1, '2011-11-11', 11806, 0, '2017-01-01 00:00:00', 0), \
('The-Bear-979', 'The best classic country music played on the east coast of Carolina', 'http://96.31.83.86:8200/', 'http://www.bear979.com/', 'The-Bear-979.png', 0, 0, 0, 1, '2011-11-11', 11807, 0, '2017-01-01 00:00:00', 0), \
('KKOY-1460', '', 'http://96.31.83.86:8200/', 'http://www.kkoy.com/', 'KKOY-1460.png', 0, 0, 0, 1, '2011-11-11', 11808, 0, '2017-01-01 00:00:00', 0), \
('Beatles-Day-Mons', 'Beatles songs and cover, new pop rock and classic rock, french songs, oldies ,french and belgian news.<BR>Lineup: Alternately Beatles/Paul McCartney/John Lennon/George Harrison/Ringo Starr/French song', 'http://96.31.83.86:8200/', 'http://www.beatlesday.eu/', 'Beatles-Day-Mons.png', 0, 0, 0, 1, '2011-11-11', 11809, 0, '2017-01-01 00:00:00', 0), \
('RadioPower', '', 'http://96.31.83.86:8200/', 'http://www.radiopower.com.pe/', 'RadioPower.png', 0, 0, 0, 1, '2011-11-11', 11810, 0, '2017-01-01 00:00:00', 0), \
('NRJ-Hit-Music-Only', '', 'http://96.31.83.86:8200/', '', 'NRJ-Hit-Music-Only.png', 0, 0, 0, 1, '2011-11-11', 11811, 0, '2017-01-01 00:00:00', 0), \
('BBC-Radio-1s-Review-with-Nihal', '', 'http://96.31.83.86:8200/', '', 'BBC-Radio-1s-Review-with-Nihal.png', 0, 0, 0, 1, '2011-11-11', 11812, 0, '2017-01-01 00:00:00', 0), \
('New-Edition', '', 'http://96.31.83.86:8200/', '', 'New-Edition.png', 0, 0, 0, 1, '2011-11-11', 11813, 0, '2017-01-01 00:00:00', 0), \
('NERadio-Nonstop', '', 'http://96.31.83.86:8200/', 'http://no.neradio.fm/', 'NERadio-Nonstop.png', 0, 0, 0, 1, '2011-11-11', 11815, 0, '2017-01-01 00:00:00', 0), \
('HighTimes-Radio', '', 'http://96.31.83.86:8200/', 'http://www.hightimesradio.org/', 'HighTimes-Radio.png', 0, 0, 0, 1, '2011-11-11', 11816, 0, '2017-01-01 00:00:00', 0), \
('Derti-FM-986', '', 'http://96.31.83.86:8200/', 'http://www.derti.gr/', 'Derti-FM-986.png', 0, 0, 0, 1, '2011-11-11', 11817, 0, '2017-01-01 00:00:00', 0), \
('B-1073', '', 'http://96.31.83.86:8200/', 'http://www.b1073.com/', 'B-1073.png', 0, 0, 0, 1, '2011-11-11', 11818, 0, '2017-01-01 00:00:00', 0), \
('FunX-Rotterdam-918', '', 'http://96.31.83.86:8200/', 'http://www.funx.nl/', 'FunX-Rotterdam-918.png', 0, 0, 0, 1, '2011-11-11', 11819, 0, '2017-01-01 00:00:00', 0), \
('The-Deep-End', '', 'http://96.31.83.86:8200/', '', 'The-Deep-End.png', 0, 0, 0, 1, '2011-11-11', 11820, 0, '2017-01-01 00:00:00', 0), \
('B-951', 'All the Hits B95.1 is middle Georgia&#39;s home for today&#39;s hit music!<BR><BR>WMGB (95.1 FM, &quot;B95.1&quot;) is a radio station serving the Macon, Georgia area with a Top 40 (CHR) format. This ', 'http://96.31.83.86:8200/', 'http://www.allthehitsb951.com/', 'B-951.png', 0, 0, 0, 1, '2011-11-11', 11821, 0, '2017-01-01 00:00:00', 0), \
('LaBelle', '', 'http://96.31.83.86:8200/', '', 'LaBelle.png', 0, 0, 0, 1, '2011-11-11', 11822, 0, '2017-01-01 00:00:00', 0), \
('KTTS-FM-947', '', 'http://96.31.83.86:8200/', 'http://ktts.com/', 'KTTS-FM-947.png', 0, 0, 0, 1, '2011-11-11', 11823, 0, '2017-01-01 00:00:00', 0), \
('Yellow-Ostrich', '', 'http://96.31.83.86:8200/', '', 'Yellow-Ostrich.png', 0, 0, 0, 1, '2011-11-11', 11824, 0, '2017-01-01 00:00:00', 0), \
('LL-Cool-J', '', 'http://96.31.83.86:8200/', '', 'LL-Cool-J.png', 0, 0, 0, 1, '2011-11-11', 11825, 0, '2017-01-01 00:00:00', 0), \
('Radiomiaminet-Los-Angeles', 'Radiomiami.net from Los Angeles, Miami and Barcelona. Radiomiami.net coast to coast in USA and around the world by internet.', 'http://96.31.83.86:8200/', 'http://www.radiomiami.net/', 'Radiomiaminet-Los-Angeles.png', 0, 0, 0, 1, '2011-11-11', 11826, 0, '2017-01-01 00:00:00', 0), \
('Radio-Sportiva-957', 'Per la prima volta in Italia una radio si rivolge ai tantissimi appassionati di sport che popolano la penisola, per fornire loro notizie ed approfondimenti in real time su tutte le realt&#224; calcist', 'http://96.31.83.86:8200/', 'http://www.radiosportiva.com/', 'Radio-Sportiva-957.png', 0, 0, 0, 1, '2011-11-11', 11827, 0, '2017-01-01 00:00:00', 0), \
('WPFW-893', '', 'http://96.31.83.86:8200/', 'http://www.wpfw.org/', 'WPFW-893.png', 0, 0, 0, 1, '2011-11-11', 11828, 0, '2017-01-01 00:00:00', 0), \
('Jazz-and-Lounge', '', 'http://96.31.83.86:8200/', 'http://jazz-and-lounge.playtheradio.com/', 'Jazz-and-Lounge.png', 0, 0, 0, 1, '2011-11-11', 11829, 0, '2017-01-01 00:00:00', 0), \
('WBET-FM-993', '', 'http://96.31.83.86:8200/', 'http://www.wmshradio.com/', 'WBET-FM-993.png', 0, 0, 0, 1, '2011-11-11', 11830, 0, '2017-01-01 00:00:00', 0), \
('Soul-Classics-1035', '', 'http://96.31.83.86:8200/', 'http://www.soulclassics.com/', 'Soul-Classics-1035.png', 0, 0, 0, 1, '2011-11-11', 11831, 0, '2017-01-01 00:00:00', 0), \
('The-Best-Mix-951', 'Trinidad first music station bringing you the best mix of music 80 until to today.', 'http://96.31.83.86:8200/', 'http://www.951thebestmix.co.tt/', 'The-Best-Mix-951.png', 0, 0, 0, 1, '2011-11-11', 11832, 0, '2017-01-01 00:00:00', 0), \
('Cannibal-Corpse', '', 'http://96.31.83.86:8200/', '', 'Cannibal-Corpse.png', 0, 0, 0, 1, '2011-11-11', 11833, 0, '2017-01-01 00:00:00', 0), \
('Big-Up-Radio-Dancehall-Reggae', 'Since 2003, internet radio stations have been among the most listened to Reggae stations online.', 'http://96.31.83.86:8200/', 'http://www.bigupradio.com/', 'Big-Up-Radio---Dancehall-Reggae.png', 0, 0, 0, 1, '2011-11-11', 11834, 0, '2017-01-01 00:00:00', 0), \
('KSCB-1270', 'Talk Radio...With the power of CNN Radio news and the areas only full time news and sports staff, KSCB-AM is the information station!', 'http://96.31.83.86:8200/', 'http://www.kscb.net/', 'KSCB-1270.png', 0, 0, 0, 1, '2011-11-11', 11835, 0, '2017-01-01 00:00:00', 0), \
('SWR3-moose-radio-996', 'SWR3 ist die multimediale Pop- Musik- und Party-Events, Comedy-Touren sowie der TV-Show', 'http://96.31.83.86:8200/', 'http://www.swr3.de/', 'SWR3-moose-radio-996.png', 0, 0, 0, 1, '2011-11-11', 11836, 0, '2017-01-01 00:00:00', 0), \
('KQNK-FM-1067', '', 'http://96.31.83.86:8200/', 'http://www.kqnk.com/', 'KQNK-FM-1067.png', 0, 0, 0, 1, '2011-11-11', 11837, 0, '2017-01-01 00:00:00', 0), \
('KCMO-FM-949', '', 'http://96.31.83.86:8200/', 'http://www.949kcmo.com/', 'KCMO-FM-949.png', 0, 0, 0, 1, '2011-11-11', 11838, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-80s-Pop', '80s pop hits from the musically transitional decade, which saw synthesizers, funk and the influence of music television creating a whole new pop', 'http://96.31.83.86:8200/', 'http://www.radioio.com/channels/80s/pop', 'RadioIO-80s-Pop.png', 0, 0, 0, 1, '2011-11-11', 11840, 0, '2017-01-01 00:00:00', 0), \
('Metro-Plus-Live-1044', 'Having Hong Kong as one of the most cosmopolitan City in the World, Metro Plus AM 1044 serves it listeners with the same concept.', 'http://96.31.83.86:8200/', 'http://www.metroradio.com.hk/1044/', 'Metro-Plus-Live-1044.png', 0, 0, 0, 1, '2011-11-11', 11842, 0, '2017-01-01 00:00:00', 0), \
('Gil-Scott-Heron', '', 'http://96.31.83.86:8200/', '', 'Gil-Scott-Heron.png', 0, 0, 0, 1, '2011-11-11', 11843, 0, '2017-01-01 00:00:00', 0), \
('Elvis-Costello', '', 'http://96.31.83.86:8200/', '', 'Elvis-Costello.png', 0, 0, 0, 1, '2011-11-11', 11844, 0, '2017-01-01 00:00:00', 0), \
('Hits-My-Music-Power', '', 'http://96.31.83.86:8200/', 'http://www.hitsmymusic.com/#!hits-my-music-power', 'Hits-My-Music-Power.png', 0, 0, 0, 1, '2011-11-11', 11845, 0, '2017-01-01 00:00:00', 0), \
('Xposure', '', 'http://96.31.83.86:8200/', '', 'Xposure.png', 0, 0, 0, 1, '2011-11-11', 11846, 0, '2017-01-01 00:00:00', 0), \
('blink-182', '', 'http://96.31.83.86:8200/', '', 'blink-182.png', 0, 0, 0, 1, '2011-11-11', 11847, 0, '2017-01-01 00:00:00', 0), \
('Hot-1071', '', 'http://96.31.83.86:8200/', 'http://www.hot1071.com/', 'Hot-1071.png', 0, 0, 0, 1, '2011-11-11', 11848, 0, '2017-01-01 00:00:00', 0), \
('Vicente-Fern¡ndez', '', 'http://96.31.83.86:8200/', '', 'Vicente-Fern¡ndez.png', 0, 0, 0, 1, '2011-11-11', 11849, 0, '2017-01-01 00:00:00', 0), \
('1027-Da-Bomb', '', 'http://96.31.83.86:8200/', 'http://www.1027dabomb.net/', '1027-Da-Bomb.png', 0, 0, 0, 1, '2011-11-11', 11850, 0, '2017-01-01 00:00:00', 0), \
('Cutting-Crew', '', 'http://96.31.83.86:8200/', '', 'Cutting-Crew.png', 0, 0, 0, 1, '2011-11-11', 11851, 0, '2017-01-01 00:00:00', 0), \
('971-Amp-Radio', 'KAMP-FM 97.1 FM, known on-air as 97.1 AMP Radio, is a Rhythmic Contemporary radio station in Los Angeles, California. The station is owned by CBS Radio.', 'http://96.31.83.86:8200/', 'http://www.ampradio.com/', '971-Amp-Radio.png', 0, 0, 0, 1, '2011-11-11', 11852, 0, '2017-01-01 00:00:00', 0), \
('Kiwi-FM-1022', '', 'http://96.31.83.86:8200/', 'http://www.kiwifm.co.nz/', 'Kiwi-FM-1022.png', 0, 0, 0, 1, '2011-11-11', 11853, 0, '2017-01-01 00:00:00', 0), \
('The-Ref-960', '', 'http://96.31.83.86:8200/', 'http://www.960theref.com', 'The-Ref-960.png', 0, 0, 0, 1, '2011-11-11', 11854, 0, '2017-01-01 00:00:00', 0), \
('KBER-101-1011', '', 'http://96.31.83.86:8200/', 'http://www.kber.com/', 'KBER-101-1011.png', 0, 0, 0, 1, '2011-11-11', 11855, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1240', '', 'http://96.31.83.86:8200/', 'http://www.eagle1023.com/wlagam/index.html', 'ESPN-1240.png', 0, 0, 0, 1, '2011-11-11', 11856, 0, '2017-01-01 00:00:00', 0), \
('WPPA-1360', '', 'http://96.31.83.86:8200/', 'http://www.wpparadio.com/', 'WPPA-1360.png', 0, 0, 0, 1, '2011-11-11', 11857, 0, '2017-01-01 00:00:00', 0), \
('KBRL-1300', '', 'http://96.31.83.86:8200/', '', 'KBRL-1300.png', 0, 0, 0, 1, '2011-11-11', 11858, 0, '2017-01-01 00:00:00', 0), \
('Dave-Edmunds', '', 'http://96.31.83.86:8200/', '', 'Dave-Edmunds.png', 0, 0, 0, 1, '2011-11-11', 11859, 0, '2017-01-01 00:00:00', 0), \
('The-TRUTH-1041', '', 'http://96.31.83.86:8200/', 'http://www.1041thetruth.com/', 'The-TRUTH-1041.png', 0, 0, 0, 1, '2011-11-11', 11860, 0, '2017-01-01 00:00:00', 0), \
('The-Real-Breakfast-Show', '', 'http://96.31.83.86:8200/', '', 'The-Real-Breakfast-Show.png', 0, 0, 0, 1, '2011-11-11', 11861, 0, '2017-01-01 00:00:00', 0), \
('Ultimate-Kaos', '', 'http://96.31.83.86:8200/', '', 'Ultimate-Kaos.png', 0, 0, 0, 1, '2011-11-11', 11862, 0, '2017-01-01 00:00:00', 0), \
('Dash-Berlin', '', 'http://96.31.83.86:8200/', '', 'Dash-Berlin.png', 0, 0, 0, 1, '2011-11-11', 11863, 0, '2017-01-01 00:00:00', 0), \
('WFHG-980', '', 'http://96.31.83.86:8200/', '', 'WFHG-980.png', 0, 0, 0, 1, '2011-11-11', 11864, 0, '2017-01-01 00:00:00', 0), \
('989-Radio-Now', '', 'http://96.31.83.86:8200/', 'http://www.989radionow.com/', '989-Radio-Now.png', 0, 0, 0, 1, '2011-11-11', 11865, 0, '2017-01-01 00:00:00', 0), \
('Gospel-Star', '', 'http://96.31.83.86:8200/', '', 'Gospel-Star.png', 0, 0, 0, 1, '2011-11-11', 11866, 0, '2017-01-01 00:00:00', 0), \
('Bush', '', 'http://96.31.83.86:8200/', '', 'Bush.png', 0, 0, 0, 1, '2011-11-11', 11867, 0, '2017-01-01 00:00:00', 0), \
('KFST-860', '', 'http://96.31.83.86:8200/', '', 'KFST-860.png', 0, 0, 0, 1, '2011-11-11', 11868, 0, '2017-01-01 00:00:00', 0), \
('Candy-Station-on-Goom', '', 'http://96.31.83.86:8200/', 'http://www.goomradio.fr/radio/candy-station', 'Candy-Station-on-Goom.png', 0, 0, 0, 1, '2011-11-11', 11869, 0, '2017-01-01 00:00:00', 0), \
('Light-of-Life', '', 'http://96.31.83.86:8200/', '', 'Light-of-Life.png', 0, 0, 0, 1, '2011-11-11', 11870, 0, '2017-01-01 00:00:00', 0), \
('B-Rock-937', '', 'http://96.31.83.86:8200/', 'http://www.brock937.com/', 'B-Rock-937.png', 0, 0, 0, 1, '2011-11-11', 11871, 0, '2017-01-01 00:00:00', 0), \
('Kiss-FM-993', '', 'http://96.31.83.86:8200/', 'http://www.993kissfm.com/', 'Kiss-FM-993.png', 0, 0, 0, 1, '2011-11-11', 11872, 0, '2017-01-01 00:00:00', 0), \
('TalkRadio-1360', '', 'http://96.31.83.86:8200/', 'http://www.wkmi.com/', 'TalkRadio-1360.png', 0, 0, 0, 1, '2011-11-11', 11873, 0, '2017-01-01 00:00:00', 0), \
('Bryan-Ferry', '', 'http://96.31.83.86:8200/', '', 'Bryan-Ferry.png', 0, 0, 0, 1, '2011-11-11', 11875, 0, '2017-01-01 00:00:00', 0), \
('WLKR-FM-953', 'The station About the Music', 'http://96.31.83.86:8200/', 'http://wlkr.northcoastnow.com/', 'WLKR-FM-953.png', 0, 0, 0, 1, '2011-11-11', 11876, 0, '2017-01-01 00:00:00', 0), \
('Midday-Jazz-with-Terry-McElligott', '', 'http://96.31.83.86:8200/', '', 'Midday-Jazz-with-Terry-McElligott.png', 0, 0, 0, 1, '2011-11-11', 11877, 0, '2017-01-01 00:00:00', 0), \
('At-The-Track-Radio-995', 'AT THE TRACK RADIO: All Motorsports All The Time.', 'http://96.31.83.86:8200/', 'http://www.us99.com/', 'At-The-Track-Radio-995.png', 0, 0, 0, 1, '2011-11-11', 11878, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Sermon', '', 'http://96.31.83.86:8200/', '', 'Sunday-Sermon.png', 0, 0, 0, 1, '2011-11-11', 11879, 0, '2017-01-01 00:00:00', 0), \
('Mix-FM-949', '', 'http://96.31.83.86:8200/', 'http://www.mixfm.com/', 'Mix-FM-949.png', 0, 0, 0, 1, '2011-11-11', 11880, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Baton-Rouge-1045', '', 'http://96.31.83.86:8200/', 'http://www.1045espn.com/', 'ESPN-Baton-Rouge-1045.png', 0, 0, 0, 1, '2011-11-11', 11881, 0, '2017-01-01 00:00:00', 0), \
('The-WOLF-1065', 'WDAF-FM is a country music radio station based in Kansas City, Missouri.', 'http://96.31.83.86:8200/', 'http://www.1065thewolf.com/', 'The-WOLF-1065.png', 0, 0, 0, 1, '2011-11-11', 11882, 0, '2017-01-01 00:00:00', 0), \
('John-Clayton-Show', '', 'http://96.31.83.86:8200/', '', 'John-Clayton-Show.png', 0, 0, 0, 1, '2011-11-11', 11883, 0, '2017-01-01 00:00:00', 0), \
('ArrayFM-Alternative-Rock', '', 'http://96.31.83.86:8200/', 'http://array.fm/altrock', 'ArrayFM-Alternative-Rock.png', 0, 0, 0, 1, '2011-11-11', 11884, 0, '2017-01-01 00:00:00', 0), \
('Pagan-Pentagram-Radio', '', 'http://96.31.83.86:8200/', 'http://paganpentagram.home.comcast.net/', 'Pagan-Pentagram-Radio.png', 0, 0, 0, 1, '2011-11-11', 11885, 0, '2017-01-01 00:00:00', 0), \
('Weekends-with-Stick-Mareebo', '', 'http://96.31.83.86:8200/', '', 'Weekends-with-Stick-Mareebo.png', 0, 0, 0, 1, '2011-11-11', 11886, 0, '2017-01-01 00:00:00', 0), \
('Krater-96-963', 'KRTR-FM (96.3 FM, &quot;KRATER 96&quot;) is an Adult Contemporary music formatted radio station serving Honolulu, Hawaii. The Cox Radio outlet broadcasts with an ERP of 74 kW and is licensed to Kailua', 'http://96.31.83.86:8200/', 'http://krater96.com/', 'Krater-96-963.png', 0, 0, 0, 1, '2011-11-11', 11887, 0, '2017-01-01 00:00:00', 0), \
('WOIC-1230', '', 'http://96.31.83.86:8200/', 'http://www.woic.com/', 'WOIC-1230.png', 0, 0, 0, 1, '2011-11-11', 11888, 0, '2017-01-01 00:00:00', 0), \
('WGKY-959', '', 'http://96.31.83.86:8200/', 'http://www.959wgky.com/', 'WGKY-959.png', 0, 0, 0, 1, '2011-11-11', 11889, 0, '2017-01-01 00:00:00', 0), \
('WELR-1360', '', 'http://96.31.83.86:8200/', 'http://www.eagle1023.com/welram/', 'WELR-1360.png', 0, 0, 0, 1, '2011-11-11', 11890, 0, '2017-01-01 00:00:00', 0), \
('Badgers-Sports', '', 'http://96.31.83.86:8200/', '', 'Badgers-Sports.png', 0, 0, 1, 1, '2011-11-11', 11891, 0, '2011-10-14 16:02:39', 0), \
('Detroit-Lions-Pre-Game-Show', '', 'http://96.31.83.86:8200/', '', 'Detroit-Lions-Pre-Game-Show.png', 0, 0, 0, 1, '2011-11-11', 11892, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-970', 'Modesto&#39;s Morning News from 5 am to 9 am. ESPN Radio all day after that.', 'http://96.31.83.86:8200/', 'http://www.espnradio970.com/', 'ESPN-Radio-970.png', 0, 0, 0, 1, '2011-11-11', 11893, 0, '2017-01-01 00:00:00', 0), \
('The-Sports-Bar', '', 'http://96.31.83.86:8200/', '', 'The-Sports-Bar.png', 0, 0, 0, 1, '2011-11-11', 11894, 0, '2017-01-01 00:00:00', 0), \
('KQ-103-1031', '', 'http://96.31.83.86:8200/', 'http://kq103.com/', 'KQ-103-1031.png', 0, 0, 0, 1, '2011-11-11', 11895, 0, '2017-01-01 00:00:00', 0), \
('Power-88-881', 'A community service of EOB, KCEP-FM provides more than 150,000 listeners each week an eclectic blend of hip hop and R&amp;B, locally produced public affairs programming and cultural programs. The stat', 'http://96.31.83.86:8200/', 'http://www.power88lv.com/', 'Power-88-881.png', 0, 0, 0, 1, '2011-11-11', 11896, 0, '2017-01-01 00:00:00', 0), \
('KBRN-1500', '', 'http://96.31.83.86:8200/', 'http://www.radioformulasa.com/', 'KBRN-1500.png', 0, 0, 0, 1, '2011-11-11', 11897, 0, '2017-01-01 00:00:00', 0), \
('Hit-Parade', '', 'http://96.31.83.86:8200/', '', 'Hit-Parade.png', 0, 0, 0, 1, '2011-11-11', 11898, 0, '2017-01-01 00:00:00', 0), \
('Matt-Costa', '', 'http://96.31.83.86:8200/', '', 'Matt-Costa.png', 0, 0, 0, 1, '2011-11-11', 11899, 0, '2017-01-01 00:00:00', 0), \
('Rob-Kacey', '', 'http://96.31.83.86:8200/', '', 'Rob-Kacey.png', 0, 0, 0, 1, '2011-11-11', 11900, 0, '2017-01-01 00:00:00', 0), \
('Blackstreet', '', 'http://96.31.83.86:8200/', '', 'Blackstreet.png', 0, 0, 0, 1, '2011-11-11', 11901, 0, '2017-01-01 00:00:00', 0), \
('Raceday', '', 'http://96.31.83.86:8200/', '', 'Raceday.png', 0, 0, 0, 1, '2011-11-11', 11902, 0, '2017-01-01 00:00:00', 0), \
('Rock-923', '', 'http://96.31.83.86:8200/', 'http://www.rock923neworleans.com/', 'Rock-923.png', 0, 0, 0, 1, '2011-11-11', 11903, 0, '2017-01-01 00:00:00', 0), \
('Mix-1063', 'WGER (106.3 FM, &quot;Mix 106.3&quot;) is a radio station licensed to Saginaw, Michigan broadcasting a hot adult contemporary format.<BR><BR>WGER can be heard in the Tri-Cities, as their niche market.', 'http://96.31.83.86:8200/', 'http://www.mix1063fm.com/', 'Mix-1063.png', 0, 0, 0, 1, '2011-11-11', 11904, 0, '2017-01-01 00:00:00', 0), \
('The-Starting-Lineup', '', 'http://96.31.83.86:8200/', '', 'The-Starting-Lineup.png', 0, 0, 0, 1, '2011-11-11', 11905, 0, '2017-01-01 00:00:00', 0), \
('BBC-Radio-1-Wales-995', '', 'http://96.31.83.86:8200/', 'http://www.bbc.co.uk/radio1/', 'BBC-Radio-1-Wales-995.png', 0, 0, 0, 1, '2011-11-11', 11906, 0, '2017-01-01 00:00:00', 0), \
('Prayer-Church', '', 'http://96.31.83.86:8200/', '', 'Prayer-Church.png', 0, 0, 0, 1, '2011-11-11', 11907, 0, '2017-01-01 00:00:00', 0), \
('Decades-101-1011', '', 'http://96.31.83.86:8200/', 'http://www.gannonbroadcasting.com/decades.html', 'Decades-101-1011.png', 0, 0, 0, 1, '2011-11-11', 11908, 0, '2017-01-01 00:00:00', 0), \
('KROX-1260', '', 'http://96.31.83.86:8200/', 'http://www.kroxam.com/', 'KROX-1260.png', 0, 0, 0, 1, '2011-11-11', 11909, 0, '2017-01-01 00:00:00', 0), \
('WKMM-967', '', 'http://96.31.83.86:8200/', 'http://www.wkmmfm.com/', 'WKMM-967.png', 0, 0, 0, 1, '2011-11-11', 11910, 0, '2017-01-01 00:00:00', 0), \
('Johnny-Lytle', '', 'http://96.31.83.86:8200/', '', 'Johnny-Lytle.png', 0, 0, 0, 1, '2011-11-11', 11911, 0, '2017-01-01 00:00:00', 0), \
('KXEO-1340', '', 'http://96.31.83.86:8200/', 'http://info.kxeo.com/', 'KXEO-1340.png', 0, 0, 0, 1, '2011-11-11', 11912, 0, '2017-01-01 00:00:00', 0), \
('Deadmau5', '', 'http://96.31.83.86:8200/', '', 'Deadmau5.png', 0, 0, 0, 1, '2011-11-11', 11914, 0, '2017-01-01 00:00:00', 0), \
('The-Hoss-991', 'One Hoss of a station... New and Traditional Country with some Rebel Country, Southern Rock and Alternative Country Mixed In. We are also our area&#39;s community connection and THE PLACE for South Ce', 'http://96.31.83.86:8200/', 'http://www.thehoss.com/', 'The-Hoss-991.png', 0, 0, 0, 1, '2011-11-11', 11915, 0, '2017-01-01 00:00:00', 0), \
('Power-1043', '', 'http://96.31.83.86:8200/', 'http://power1043.com/', 'Power-1043.png', 0, 0, 0, 1, '2011-11-11', 11917, 0, '2017-01-01 00:00:00', 0), \
('Other-Lives', '', 'http://96.31.83.86:8200/', '', 'Other-Lives.png', 0, 0, 0, 1, '2011-11-11', 11918, 0, '2017-01-01 00:00:00', 0), \
('Texas-99-991', '', 'http://96.31.83.86:8200/', 'http://www.texas99.com/', 'Texas-99-991.png', 0, 0, 0, 1, '2011-11-11', 11919, 0, '2017-01-01 00:00:00', 0), \
('KJNO-630', '', 'http://96.31.83.86:8200/', 'http://www.kjno.com/', 'KJNO-630.png', 0, 0, 0, 1, '2011-11-11', 11920, 0, '2017-01-01 00:00:00', 0), \
('Czechoslovak-Sunday-Radio-Hour', '', 'http://96.31.83.86:8200/', '', 'Czechoslovak-Sunday-Radio-Hour.png', 0, 0, 0, 1, '2011-11-11', 11921, 0, '2017-01-01 00:00:00', 0), \
('NRJ-Love', '', 'http://96.31.83.86:8200/', 'http://www.energyzueri.ch/', 'NRJ-Love.png', 0, 0, 0, 1, '2011-11-11', 11922, 0, '2017-01-01 00:00:00', 0), \
('KZNU-1450', '', 'http://96.31.83.86:8200/', 'http://www.foxnews1450.com/', 'KZNU-1450.png', 0, 0, 0, 1, '2011-11-11', 11923, 0, '2017-01-01 00:00:00', 0), \
('MEGA-949', '', 'http://96.31.83.86:8200/', 'http://www.mega949.com/', 'MEGA-949.png', 0, 0, 0, 1, '2011-11-11', 11924, 0, '2017-01-01 00:00:00', 0), \
('Nicola-Conte', '', 'http://96.31.83.86:8200/', '', 'Nicola-Conte.png', 0, 0, 0, 1, '2011-11-11', 11925, 0, '2017-01-01 00:00:00', 0), \
('Bremen-Eins-938', 'Radio from Bremen', 'http://96.31.83.86:8200/', 'http://www.radiobremen.de/bremeneins/', 'Bremen-Eins-938.png', 0, 0, 0, 1, '2011-11-11', 11926, 0, '2017-01-01 00:00:00', 0), \
('Good-Charlotte', '', 'http://96.31.83.86:8200/', '', 'Good-Charlotte.png', 0, 0, 0, 1, '2011-11-11', 11927, 0, '2017-01-01 00:00:00', 0), \
('P4-Radio-Ballade', '', 'http://96.31.83.86:8200/', 'http://www.p4.no/', 'P4-Radio-Ballade.png', 0, 0, 0, 1, '2011-11-11', 11928, 0, '2017-01-01 00:00:00', 0), \
('Radio-Fiesta', '', 'http://96.31.83.86:8200/', '', 'Radio-Fiesta.png', 0, 0, 0, 1, '2011-11-11', 11932, 0, '2017-01-01 00:00:00', 0), \
('The-Fox-1039', '', 'http://96.31.83.86:8200/', 'http://www.classicfox.com/', 'The-Fox-1039.png', 0, 0, 0, 1, '2011-11-11', 11933, 0, '2017-01-01 00:00:00', 0), \
('Radio-Limfjord', '', 'http://96.31.83.86:8200/', '', 'Radio-Limfjord.png', 0, 0, 0, 1, '2011-11-11', 11934, 0, '2017-01-01 00:00:00', 0), \
('4U-Rock-N-Metal', 'Hard Rock, Heavy Metal, Progressive Metal et tous les h&#233;ros de la guitare sont sur 4U-Rock N Metal ! Joe Satriani, Steve Vai, John Petrucci, AC/DC, Metallica, Iron Maiden, Slayer, Dream Theater, ', 'http://96.31.83.86:8200/', 'http://www.4urocknmetal.com/', '4U-Rock-N-Metal.png', 0, 0, 0, 1, '2011-11-11', 11935, 0, '2017-01-01 00:00:00', 0), \
('Jimmy-Cliff', '', 'http://96.31.83.86:8200/', '', 'Jimmy-Cliff.png', 0, 0, 0, 1, '2011-11-11', 11936, 0, '2017-01-01 00:00:00', 0), \
('Radio-Morioka-769', '', 'http://96.31.83.86:8200/', 'http://www.radiomorioka.co.jp/', 'Radio-Morioka-769.png', 0, 0, 0, 1, '2011-11-11', 11937, 0, '2017-01-01 00:00:00', 0), \
('September', '', 'http://96.31.83.86:8200/', '', 'September.png', 0, 0, 0, 1, '2011-11-11', 11938, 0, '2017-01-01 00:00:00', 0), \
('Little-Guys-Home-Electronics-Show', '', 'http://96.31.83.86:8200/', '', 'Little-Guys-Home-Electronics-Show.png', 0, 0, 0, 1, '2011-11-11', 11939, 0, '2017-01-01 00:00:00', 0), \
('KBULL-981', '', 'http://96.31.83.86:8200/', 'http://www.kbul.com/', 'KBULL-981.png', 0, 0, 0, 1, '2011-11-11', 11940, 0, '2017-01-01 00:00:00', 0), \
('WATR-1320', '', 'http://96.31.83.86:8200/', 'http://www.watr.com/', 'WATR-1320.png', 0, 0, 0, 1, '2011-11-11', 11941, 0, '2017-01-01 00:00:00', 0), \
('The-Game-900', '', 'http://96.31.83.86:8200/', 'http://www.wgamradio.com/', 'The-Game-900.png', 0, 0, 0, 1, '2011-11-11', 11942, 0, '2017-01-01 00:00:00', 0), \
('KPFA-941', 'Founded in 1949 by Lewis Hill, a pacifist, poet, and journalist, KPFA was the first community supported radio station in the USA', 'http://96.31.83.86:8200/', 'http://www.kpfa.org/', 'KPFA-941.png', 0, 0, 0, 1, '2011-11-11', 11943, 0, '2017-01-01 00:00:00', 0), \
('Gina-Valente', '', 'http://96.31.83.86:8200/', '', 'Gina-Valente.png', 0, 0, 0, 1, '2011-11-11', 11944, 0, '2017-01-01 00:00:00', 0), \
('3WZ-953', '3WZ IS FULL-SERVICE RADIO. Your home for Adult Hit Music... CBS Radio News...', 'http://96.31.83.86:8200/', 'http://www.3wz.com/', '3WZ-953.png', 0, 0, 0, 1, '2011-11-11', 11945, 0, '2017-01-01 00:00:00', 0), \
('Kiss-FM-Lebanon-1049', 'Broadcasting From the Capitol of Lebanon, Beirut City. We Play The Best Hits of the 1980s and 1990s', 'http://96.31.83.86:8200/', 'http://www.kissfmlebanon.info', 'Kiss-FM-Lebanon-1049.png', 0, 0, 0, 1, '2011-11-11', 11946, 0, '2017-01-01 00:00:00', 0), \
('The-Fast-Track', '', 'http://96.31.83.86:8200/', '', 'The-Fast-Track.png', 0, 0, 0, 1, '2011-11-11', 11947, 0, '2017-01-01 00:00:00', 0), \
('Private-Investigations', '24 hours webradio.', 'http://96.31.83.86:8200/', 'http://www.radionomy.com/en/radio/private-investigations', 'Private-Investigations.png', 0, 0, 0, 1, '2011-11-11', 11948, 0, '2017-01-01 00:00:00', 0), \
('Todays-Business', '', 'http://96.31.83.86:8200/', '', 'Todays-Business.png', 0, 0, 0, 1, '2011-11-11', 11949, 0, '2017-01-01 00:00:00', 0), \
('The-Oasis-1057', '', 'http://96.31.83.86:8200/', 'http://www.1057theoasis.com/', 'The-Oasis-1057.png', 0, 0, 0, 1, '2011-11-11', 11950, 0, '2017-01-01 00:00:00', 0), \
('KNET-1450', 'All the top talk shows during the weekday along with ESPN sports at night.  Then on the weekend tons of variety!', 'http://96.31.83.86:8200/', 'http://www.youreasttexas.com/KNET-95-7FM---1450AM/7646302', 'KNET-1450.png', 0, 0, 0, 1, '2011-11-11', 11951, 0, '2017-01-01 00:00:00', 0), \
('97-LAV-FM-969', '', 'http://96.31.83.86:8200/', 'http://www.wlav.com/', '97-LAV-FM-969.png', 0, 0, 0, 1, '2011-11-11', 11952, 0, '2017-01-01 00:00:00', 0), \
('DZAS-702', '', 'http://96.31.83.86:8200/', 'http://www.febc.ph/stations/dzas/index.html', 'DZAS-702.png', 0, 0, 0, 1, '2011-11-11', 11953, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-935', '', 'http://96.31.83.86:8200/', 'http://www.935thewolf.com/', 'The-Wolf-935.png', 0, 0, 0, 1, '2011-11-11', 11954, 0, '2017-01-01 00:00:00', 0), \
('99X-997', '', 'http://96.31.83.86:8200/', 'http://www.99x.com/', '99X-997.png', 0, 0, 0, 1, '2011-11-11', 11955, 0, '2017-01-01 00:00:00', 0), \
('SportsJuice-Joliet-JackHammers', '', 'http://96.31.83.86:8200/', '', 'SportsJuice-Joliet-JackHammers.png', 0, 0, 0, 1, '2011-11-11', 11956, 0, '2017-01-01 00:00:00', 0), \
('The-Kevin-Trudeau-Show', '', 'http://96.31.83.86:8200/', '', 'The-Kevin-Trudeau-Show.png', 0, 0, 0, 1, '2011-11-11', 11957, 0, '2017-01-01 00:00:00', 0), \
('More-FM-1047', '', 'http://96.31.83.86:8200/', 'http://www.morefmtrinidad.com/', 'More-FM-1047.png', 0, 0, 0, 1, '2011-11-11', 11958, 0, '2017-01-01 00:00:00', 0), \
('Weekend-Breakfast', '', 'http://96.31.83.86:8200/', '', 'Weekend-Breakfast.png', 0, 0, 0, 1, '2011-11-11', 11959, 0, '2017-01-01 00:00:00', 0), \
('WMKV-893', '', 'http://96.31.83.86:8200/', 'http://www.wmkvfm.org/', 'WMKV-893.png', 0, 0, 0, 1, '2011-11-11', 11960, 0, '2017-01-01 00:00:00', 0), \
('Mix-1079', '', 'http://96.31.83.86:8200/', 'http://www.mixmaine.com/', 'Mix-1079.png', 0, 0, 0, 1, '2011-11-11', 11961, 0, '2017-01-01 00:00:00', 0), \
('Antenne-Bayern-Chillout', '', 'http://96.31.83.86:8200/', 'http://www.antenne.de/', 'Antenne-Bayern-Chillout.png', 0, 0, 0, 1, '2011-11-11', 11962, 0, '2017-01-01 00:00:00', 0), \
('Lincs-FM-1022', 'Lincs FM playing music from the 60', 'http://96.31.83.86:8200/', 'http://www.lincsfm.co.uk/', 'Lincs-FM-1022.png', 0, 0, 0, 1, '2011-11-11', 11963, 0, '2017-01-01 00:00:00', 0), \
('The-Sounds', '', 'http://96.31.83.86:8200/', '', 'The-Sounds.png', 0, 0, 0, 1, '2011-11-11', 11964, 0, '2017-01-01 00:00:00', 0), \
('Sermon', '', 'http://96.31.83.86:8200/', '', 'Sermon.png', 0, 0, 0, 1, '2011-11-11', 11965, 0, '2017-01-01 00:00:00', 0), \
('Fox-Oldies-1220', '', 'http://96.31.83.86:8200/', 'http://www.foxradio.net/', 'Fox-Oldies-1220.png', 0, 0, 0, 1, '2011-11-11', 11966, 0, '2017-01-01 00:00:00', 0), \
('Good-Evening-Kaybayan', '', 'http://96.31.83.86:8200/', '', 'Good-Evening-Kaybayan.png', 0, 0, 0, 1, '2011-11-11', 11968, 0, '2017-01-01 00:00:00', 0), \
('Ben-FM-957', '95.7 BEN-FM is Philadelphia&#39;s Adult Hits station with the largest music library in town. BEN-FM plays the best variety of music, including Tom Petty, Bon Jovi, Pink, Boston, 3 Doors Down, KC &amp;', 'http://96.31.83.86:8200/', 'http://www.ilikebenfm.com/', 'Ben-FM-957.png', 0, 0, 0, 1, '2011-11-11', 11969, 0, '2017-01-01 00:00:00', 0), \
('WCBS-FM-1011', '101.1 WCBS FM, playing New York Greatest Hits of the 60 classic hits station in New York!', 'http://96.31.83.86:8200/', 'http://www.wcbsfm.com/', 'WCBS-FM-1011.png', 0, 0, 0, 1, '2011-11-11', 11970, 0, '2017-01-01 00:00:00', 0), \
('Real-Country-999', '', 'http://96.31.83.86:8200/', 'http://www.realcountry999.com/', 'Real-Country-999.png', 0, 0, 0, 1, '2011-11-11', 11971, 0, '2017-01-01 00:00:00', 0), \
('Dancegroove-Radio', '', 'http://96.31.83.86:8200/', 'http://www.dancegrooveradio.com/', 'Dancegroove-Radio.png', 0, 0, 0, 1, '2011-11-11', 11972, 0, '2017-01-01 00:00:00', 0), \
('ENERGY-AT-THE-CLUB', '', 'http://96.31.83.86:8200/', '', 'ENERGY-AT-THE-CLUB.png', 0, 0, 0, 1, '2011-11-11', 11973, 0, '2017-01-01 00:00:00', 0), \
('Live-959', '', 'http://96.31.83.86:8200/', 'http://www.live959.com/', 'Live-959.png', 0, 0, 0, 1, '2011-11-11', 11974, 0, '2017-01-01 00:00:00', 0), \
('Jazz88-FM-883', '', 'http://96.31.83.86:8200/', 'http://www.wbgo.org/', 'Jazz88-FM-883.png', 0, 0, 0, 1, '2011-11-11', 11975, 0, '2017-01-01 00:00:00', 0), \
('Clyde-1-1025', '', 'http://96.31.83.86:8200/', 'http://www.clyde1.com/', 'Clyde-1-1025.png', 0, 0, 0, 1, '2011-11-11', 11976, 0, '2017-01-01 00:00:00', 0), \
('Lolli-Radio---Happy-Station', 'La stazione ', 'http://96.31.83.86:8200/', 'http://www.lolliradio.net/', 'Lolli-Radio-Happy-Station.png', 0, 0, 0, 1, '2011-11-11', 11977, 0, '2017-01-01 00:00:00', 0), \
('The-Scott-Van-Pelt-Show', '', 'http://96.31.83.86:8200/', '', 'The-Scott-Van-Pelt-Show.png', 0, 0, 0, 1, '2011-11-11', 11978, 0, '2017-01-01 00:00:00', 0), \
('WLGN-1510', '', 'http://96.31.83.86:8200/', '', 'WLGN-1510.png', 0, 0, 0, 1, '2011-11-11', 11979, 0, '2017-01-01 00:00:00', 0), \
('92-Moose-FM-923', '', 'http://96.31.83.86:8200/', 'http://www.92moose.fm/', '92-Moose-FM-923.png', 0, 0, 0, 1, '2011-11-11', 11980, 0, '2017-01-01 00:00:00', 0), \
('Sherwood-Radio', '', 'http://96.31.83.86:8200/', 'http://www.sherwoodradio.com/', 'Sherwood-Radio.png', 0, 0, 0, 1, '2011-11-11', 11981, 0, '2017-01-01 00:00:00', 0), \
('KHOZ-FM-1029', '', 'http://96.31.83.86:8200/', 'http://www.khoz.com/', 'KHOZ-FM-1029.png', 0, 0, 0, 1, '2011-11-11', 11982, 0, '2017-01-01 00:00:00', 0), \
('WHUD-1007', 'WHUD is an Adult Contemporary radio station licensed to Peekskill, New York. The station is owned by Pamal Broadcasting and broadcasts on 100.7 MHz at 50 kW ERP from a tower site in Philipstown, New Y', 'http://96.31.83.86:8200/', 'http://www.whud.com/', 'WHUD-1007.png', 0, 0, 0, 1, '2011-11-11', 11983, 0, '2017-01-01 00:00:00', 0), \
('WEAL-1510', '', 'http://96.31.83.86:8200/', 'http://www.1510weal.com/', 'WEAL-1510.png', 0, 0, 0, 1, '2011-11-11', 11984, 0, '2017-01-01 00:00:00', 0), \
('Meredith-Brooks', '', 'http://96.31.83.86:8200/', '', 'Meredith-Brooks.png', 0, 0, 0, 1, '2011-11-11', 11985, 0, '2017-01-01 00:00:00', 0), \
('Radio-Horsens-911', '', 'http://96.31.83.86:8200/', 'http://www.radiohorsens.dk/', 'Radio-Horsens-911.png', 0, 0, 0, 1, '2011-11-11', 11986, 0, '2017-01-01 00:00:00', 0), \
('Stump-the-Professor', '', 'http://96.31.83.86:8200/', '', 'Stump-the-Professor.png', 0, 0, 0, 1, '2011-11-11', 11987, 0, '2017-01-01 00:00:00', 0), \
('Womack--Womack', '', 'http://96.31.83.86:8200/', '', 'Womack--Womack.png', 0, 0, 0, 1, '2011-11-11', 11988, 0, '2017-01-01 00:00:00', 0), \
('Mix-945', 'WLRW 94.5 FM, Today&#39;s Best Music! WLRW 94.5 features Champaign area music events, contests, concerts, clubs and more!', 'http://96.31.83.86:8200/', 'http://www.mix945.com/', 'Mix-945.png', 0, 0, 0, 1, '2011-11-11', 11989, 0, '2017-01-01 00:00:00', 0), \
('Boxing-Illustrated', '', 'http://96.31.83.86:8200/', '', 'Boxing-Illustrated.png', 0, 0, 0, 1, '2011-11-11', 11990, 0, '2017-01-01 00:00:00', 0), \
('Legends-of-Success', '', 'http://96.31.83.86:8200/', '', 'Legends-of-Success.png', 0, 0, 0, 1, '2011-11-11', 11991, 0, '2017-01-01 00:00:00', 0), \
('Editor', '', 'http://96.31.83.86:8200/', '', 'Editor.png', 0, 0, 0, 1, '2011-11-11', 11992, 0, '2017-01-01 00:00:00', 0), \
('The-Arrow-937', '', 'http://96.31.83.86:8200/', 'http://www.kkrw.com/', 'The-Arrow-937.png', 0, 0, 0, 1, '2011-11-11', 11993, 0, '2017-01-01 00:00:00', 0), \
('Mix-107-9-1079', '', 'http://96.31.83.86:8200/', 'http://www.themix1079.com/', 'Mix-107-9-1079.png', 0, 0, 0, 1, '2011-11-11', 11994, 0, '2017-01-01 00:00:00', 0), \
('Kiss-FM-1027', '', 'http://96.31.83.86:8200/', 'http://www.wksb.com/', 'Kiss-FM-1027.png', 0, 0, 0, 1, '2011-11-11', 11995, 0, '2017-01-01 00:00:00', 0), \
('MSA-Sports-PIHL-Hockey-Channel-2', '', 'http://96.31.83.86:8200/', 'http://www.msasports.net', 'MSA-Sports-PIHL-Hockey-Channel-2.png', 0, 0, 0, 1, '2011-11-11', 11997, 0, '2017-01-01 00:00:00', 0), \
('Emerson-Drive', '', 'http://96.31.83.86:8200/', '', 'Emerson-Drive.png', 0, 0, 0, 1, '2011-11-11', 11998, 0, '2017-01-01 00:00:00', 0), \
('963-VIP', '', 'http://96.31.83.86:8200/', 'http://www.963vip.com/', '963-VIP.png', 0, 0, 0, 1, '2011-11-11', 11999, 0, '2017-01-01 00:00:00', 0), \
('Shooter-Jennings', '', 'http://96.31.83.86:8200/', '', 'Shooter-Jennings.png', 0, 0, 0, 1, '2011-11-11', 12000, 0, '2017-01-01 00:00:00', 0), \
('Sports-Radio-1240', '', 'http://96.31.83.86:8200/', 'http://www.1240theticket.com/', 'Sports-Radio-1240.png', 0, 0, 0, 1, '2011-11-11', 12001, 0, '2017-01-01 00:00:00', 0), \
('Rizzo-on-the-Radio-on-ESPN', '', 'http://96.31.83.86:8200/', '', 'Rizzo-on-the-Radio-on-ESPN.png', 0, 0, 0, 1, '2011-11-11', 12002, 0, '2017-01-01 00:00:00', 0), \
('Radio-Polskie---Hot-100', 'Hundred of the hottest current hits of the species: pop, dance, r&#39;n&#39;b, rock. Always fresh, always new, always the best hits non-stop, let me give you rozgrzeja.', 'http://96.31.83.86:8200/', 'http://www.polskastacja.pl/', 'Radio-Polskie---Hot-100.png', 0, 0, 0, 1, '2011-11-11', 12003, 0, '2017-01-01 00:00:00', 0), \
('Radio-Musiquera-931', '', 'http://96.31.83.86:8200/', 'http://www.musiquera.com/', 'Radio-Musiquera-931.png', 0, 0, 0, 1, '2011-11-11', 12004, 0, '2017-01-01 00:00:00', 0), \
('Inside-Pitch', '', 'http://96.31.83.86:8200/', '', 'Inside-Pitch.png', 0, 0, 0, 1, '2011-11-11', 12005, 0, '2017-01-01 00:00:00', 0), \
('The-Voice-1230', '', 'http://96.31.83.86:8200/', 'http://www.wxco.com/', 'The-Voice-1230.png', 0, 0, 0, 1, '2011-11-11', 12006, 0, '2017-01-01 00:00:00', 0), \
('CLASSIC-METAL-RADIO', 'Hard &amp; Heavy Sounds From The 20th Century &amp; Beyond', 'http://96.31.83.86:8200/', 'http://loudcaster.com/channels/35', 'CLASSIC-METAL-RADIO.png', 0, 0, 0, 1, '2011-11-11', 12007, 0, '2017-01-01 00:00:00', 0), \
('Dizzy-Gillespie', '', 'http://96.31.83.86:8200/', '', 'Dizzy-Gillespie.png', 0, 0, 0, 1, '2011-11-11', 12008, 0, '2017-01-01 00:00:00', 0), \
('The-Joey-Jones-Show', '', 'http://96.31.83.86:8200/', '', 'The-Joey-Jones-Show.png', 0, 0, 0, 1, '2011-11-11', 12009, 0, '2017-01-01 00:00:00', 0), \
('WPCC-1410', '', 'http://96.31.83.86:8200/', 'http://www.sportsradio1410wpcc.com/', 'WPCC-1410.png', 0, 0, 0, 1, '2011-11-11', 12010, 0, '2017-01-01 00:00:00', 0), \
('Radio-Rugby-Club-XV', '', 'http://96.31.83.86:8200/', 'http://www.radio-rugby.com/', 'Radio-Rugby-Club-XV.png', 0, 0, 0, 1, '2011-11-11', 12011, 0, '2017-01-01 00:00:00', 0), \
('ORF-FM-4-1038', '', 'http://96.31.83.86:8200/', 'http://fm4.orf.at/', 'ORF-FM-4-1038.png', 0, 0, 0, 1, '2011-11-11', 12012, 0, '2017-01-01 00:00:00', 0), \
('Alternative-HD', '', 'http://96.31.83.86:8200/', 'http://www.alternativehd.com/', 'Alternative-HD.png', 0, 0, 0, 1, '2011-11-11', 12013, 0, '2017-01-01 00:00:00', 0), \
('Hare-Krishna-Internet-Radio-Bhajan', '', 'http://96.31.83.86:8200/', 'http://audiokrishna.com/stations/', 'Hare-Krishna-Internet-Radio-Bhajan.png', 0, 0, 0, 1, '2011-11-11', 12014, 0, '2017-01-01 00:00:00', 0), \
('KWAM-990', '', 'http://96.31.83.86:8200/', 'http://www.kwam990.com/', 'KWAM-990.png', 0, 0, 0, 1, '2011-11-11', 12015, 0, '2017-01-01 00:00:00', 0), \
('Kid-Rock', '', 'http://96.31.83.86:8200/', '', 'Kid-Rock.png', 0, 0, 0, 1, '2011-11-11', 12016, 0, '2017-01-01 00:00:00', 0), \
('Bradenton-Marauders-Baseball-Network', '', 'http://96.31.83.86:8200/', '', 'Bradenton-Marauders-Baseball-Network.png', 0, 0, 0, 1, '2011-11-11', 12017, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Sports-SENtral', '', 'http://96.31.83.86:8200/', '', 'Sunday-Sports-SENtral.png', 0, 0, 0, 1, '2011-11-11', 12018, 0, '2017-01-01 00:00:00', 0), \
('Folk-Radio-UK-Frukie', '', 'http://96.31.83.86:8200/', 'http://www.folkradio.co.uk/', 'Folk-Radio-UK-Frukie.png', 0, 0, 0, 1, '2011-11-11', 12019, 0, '2017-01-01 00:00:00', 0), \
('Greg-James', '', 'http://96.31.83.86:8200/', '', 'Greg-James.png', 0, 0, 0, 1, '2011-11-11', 12020, 0, '2017-01-01 00:00:00', 0), \
('Mix-1063-FM', '', 'http://96.31.83.86:8200/', 'http://www.mix106.com.au/', 'Mix-1063-FM.png', 0, 0, 0, 1, '2011-11-11', 12021, 0, '2017-01-01 00:00:00', 0), \
('Variety-Rock-1051', '', 'http://96.31.83.86:8200/', 'http://www.varietyrocks.com/', 'Variety-Rock-1051.png', 0, 0, 0, 1, '2011-11-11', 12022, 0, '2017-01-01 00:00:00', 0), \
('Sublime', '', 'http://96.31.83.86:8200/', '', 'Sublime.png', 0, 0, 0, 1, '2011-11-11', 12023, 0, '2017-01-01 00:00:00', 0), \
('Your-1063', 'Your Music, Your Station, Your 1063! Listen live right now at your1063.com!', 'http://96.31.83.86:8200/', 'http://www.your1063.com/', 'Your-1063.png', 0, 0, 0, 1, '2011-11-11', 12024, 0, '2017-01-01 00:00:00', 0), \
('WPBQ-1240', '', 'http://96.31.83.86:8200/', 'http://www.espnradio1240.com/', 'WPBQ-1240.png', 0, 0, 0, 1, '2011-11-11', 12025, 0, '2017-01-01 00:00:00', 0), \
('Sleep-Reading-Stories-Poems-Music', '', 'http://96.31.83.86:8200/', '', 'Sleep-Reading-Stories-Poems-Music.png', 0, 0, 0, 1, '2011-11-11', 12026, 0, '2017-01-01 00:00:00', 0), \
('Miller-Lite-Big-Show', '', 'http://96.31.83.86:8200/', '', 'Miller-Lite-Big-Show.png', 0, 0, 0, 1, '2011-11-11', 12027, 0, '2017-01-01 00:00:00', 0), \
('In-Flames', '', 'http://96.31.83.86:8200/', '', 'In-Flames.png', 0, 0, 0, 1, '2011-11-11', 12028, 0, '2017-01-01 00:00:00', 0), \
('Golden-Hits-Radio', 'Golden Hits Radio plays 24/7 non-stop classic oldies from the 50s - 70s, commercial free, and hosted by legendary radio personalities Burt &amp;  Kurt.Golden Hits Radio plays 24/7 non-stop classic old', 'http://96.31.83.86:8200/', 'http://www.goldenhitsradio.com/', 'Golden-Hits-Radio.png', 0, 0, 0, 1, '2011-11-11', 12029, 0, '2017-01-01 00:00:00', 0), \
('941-The-Loon', '', 'http://96.31.83.86:8200/', 'http://www.kkln.com/', '941-The-Loon.png', 0, 0, 0, 1, '2011-11-11', 12030, 0, '2017-01-01 00:00:00', 0);";


const char *radio_station_setupsql37="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('The-Bull-949', '', 'http://96.31.83.86:8200/', 'http://www.949thebull.com/main.html', 'The-Bull-949.png', 0, 0, 0, 1, '2011-11-11', 12032, 0, '2017-01-01 00:00:00', 0), \
('Cross-Counties-Radio', 'Community radio from the heart of Leicestershire. Local and World news, sport, weather, travel and what&#39;s on guide. Interviews with local organisations and people. Outside broadcasts from local ev', 'http://96.31.83.86:8200/', 'http://www.crosscountiesradio.co.uk/', 'Cross-Counties-Radio.png', 0, 0, 0, 1, '2011-11-11', 12033, 0, '2017-01-01 00:00:00', 0), \
('Everything-but-the-Girl', '', 'http://96.31.83.86:8200/', '', 'Everything-but-the-Girl.png', 0, 0, 0, 1, '2011-11-11', 12034, 0, '2017-01-01 00:00:00', 0), \
('KEYB-1079', '', 'http://96.31.83.86:8200/', 'http://www.keyb.net/', 'KEYB-1079.png', 0, 0, 0, 1, '2011-11-11', 12035, 0, '2017-01-01 00:00:00', 0), \
('Bank-of-America-500-Oct-15-2011', '', 'http://96.31.83.86:8200/', '', 'Bank-of-America-500-Oct-15-2011.png', 0, 0, 0, 1, '2011-11-11', 12036, 0, '2017-01-01 00:00:00', 0), \
('955-Jazz', '', 'http://96.31.83.86:8200/', 'http://www.955jazz.com/', '955-Jazz.png', 0, 0, 0, 1, '2011-11-11', 12037, 0, '2017-01-01 00:00:00', 0), \
('Sport-on-BBC-Bristol', '', 'http://96.31.83.86:8200/', '', 'Sport-on-BBC-Bristol.png', 0, 0, 0, 1, '2011-11-11', 12038, 0, '2017-01-01 00:00:00', 0), \
('Radio-10-Gold-60s--70s-Hits', '', 'http://96.31.83.86:8200/', 'http://www.radio10gold.nl/', 'Radio-10-Gold-60s--70s-Hits.png', 0, 0, 0, 1, '2011-11-11', 12039, 0, '2017-01-01 00:00:00', 0), \
('All-Star-Radio-1400', '', 'http://96.31.83.86:8200/', 'http://www.wccy.com/', 'All-Star-Radio-1400.png', 0, 0, 0, 1, '2011-11-11', 12040, 0, '2017-01-01 00:00:00', 0), \
('DI-Hardstyle', 'Banging Hardstyle for your ears!!!', 'http://96.31.83.86:8200/', 'http://www.di.fm/hardstyle', 'DI-Hardstyle.png', 0, 0, 0, 1, '2011-11-11', 12041, 0, '2017-01-01 00:00:00', 0), \
('Hot-Summer-Sheep', '', 'http://96.31.83.86:8200/', '', 'Hot-Summer-Sheep.png', 0, 0, 0, 1, '2011-11-11', 12042, 0, '2017-01-01 00:00:00', 0), \
('KGWA-960', '', 'http://96.31.83.86:8200/', 'http://www.kgwanews.com/', 'KGWA-960.png', 0, 0, 0, 1, '2011-11-11', 12043, 0, '2017-01-01 00:00:00', 0), \
('The-White-Stripes', '', 'http://96.31.83.86:8200/', '', 'The-White-Stripes.png', 0, 0, 0, 1, '2011-11-11', 12044, 0, '2017-01-01 00:00:00', 0), \
('Mannheim-Steamroller', '', 'http://96.31.83.86:8200/', '', 'Mannheim-Steamroller.png', 0, 0, 0, 1, '2011-11-11', 12045, 0, '2017-01-01 00:00:00', 0), \
('Westsound-FM-970', '', 'http://96.31.83.86:8200/', 'http://www.westsoundradio.com/', 'Westsound-FM-970.png', 0, 0, 0, 1, '2011-11-11', 12046, 0, '2017-01-01 00:00:00', 0), \
('From-The-Press-Box-To-Press-Row', '', 'http://96.31.83.86:8200/', '', 'From-The-Press-Box-To-Press-Row.png', 0, 0, 0, 1, '2011-11-11', 12048, 0, '2017-01-01 00:00:00', 0), \
('KPOJ-620', '', 'http://96.31.83.86:8200/', 'http://www.620kpoj.com/', 'KPOJ-620.png', 0, 0, 0, 1, '2011-11-11', 12049, 0, '2017-01-01 00:00:00', 0), \
('DRS-2-990', '', 'http://96.31.83.86:8200/', 'http://www.drs2.ch/', 'DRS-2-990.png', 0, 0, 0, 1, '2011-11-11', 12050, 0, '2017-01-01 00:00:00', 0), \
('Brazilian-Fantasy', '', 'http://96.31.83.86:8200/', '', 'Brazilian-Fantasy.png', 0, 0, 0, 1, '2011-11-11', 12051, 0, '2017-01-01 00:00:00', 0), \
('New-Normal-Music', '', 'http://96.31.83.86:8200/', 'http://www.newnormalmusic.com/', 'New-Normal-Music.png', 0, 0, 0, 1, '2011-11-11', 12052, 0, '2017-01-01 00:00:00', 0), \
('WGHN-1370', 'WGHN (AM 1370) became an ESPN affiliate in January 2008.', 'http://96.31.83.86:8200/', 'http://www.sportsradio1370.com/', 'WGHN-1370.png', 0, 0, 0, 1, '2011-11-11', 12053, 0, '2017-01-01 00:00:00', 0), \
('NRJ-Dance', 'Hit Music Only!', 'http://96.31.83.86:8200/', 'http://www.energy.de', 'NRJ-Dance.png', 0, 0, 0, 1, '2011-11-11', 12054, 0, '2017-01-01 00:00:00', 0), \
('SKYFM-Pop-Punk', 'A fusion of Punk and Pop!', 'http://96.31.83.86:8200/', 'http://www.sky.fm/poppunk', 'SKYFM-Pop-Punk.png', 0, 0, 0, 1, '2011-11-11', 12055, 0, '2017-01-01 00:00:00', 0), \
('Switchfoot', '', 'http://96.31.83.86:8200/', '', 'Switchfoot.png', 0, 0, 0, 1, '2011-11-11', 12056, 0, '2017-01-01 00:00:00', 0), \
('SSRadio-Deep-and-Soulful', 'Launched in June 2005, SSRadio (SSR) is at the cutting edge of online radio. Dedicated solely to providing our listeners with the finest House music from around the world, SSR showcases exclusive show', 'http://96.31.83.86:8200/', 'http://ssradiouk.com/', 'SSRadio-Deep-and-Soulful.png', 0, 0, 0, 1, '2011-11-11', 12057, 0, '2017-01-01 00:00:00', 0), \
('Scott-Korzenowski', '', 'http://96.31.83.86:8200/', '', 'Scott-Korzenowski.png', 0, 0, 0, 1, '2011-11-11', 12058, 0, '2017-01-01 00:00:00', 0), \
('KETR-889', '', 'http://96.31.83.86:8200/', 'http://www.ketr.org/', 'KETR-889.png', 0, 0, 0, 1, '2011-11-11', 12059, 0, '2017-01-01 00:00:00', 0), \
('Hometown-News', '', 'http://96.31.83.86:8200/', '', 'Hometown-News.png', 0, 0, 0, 1, '2011-11-11', 12060, 0, '2017-01-01 00:00:00', 0), \
('Only-90s', '', 'http://96.31.83.86:8200/', 'http://www.only90s.com/', 'Only-90s.png', 0, 0, 0, 1, '2011-11-11', 12061, 0, '2017-01-01 00:00:00', 0), \
('Minimal-Wave', '', 'http://96.31.83.86:8200/', '', 'Minimal-Wave.png', 0, 0, 0, 1, '2011-11-11', 12062, 0, '2017-01-01 00:00:00', 0), \
('Energy-FM-883', '', 'http://96.31.83.86:8200/', 'http://www.energy883.gr/', 'Energy-FM-883.png', 0, 0, 0, 1, '2011-11-11', 12063, 0, '2017-01-01 00:00:00', 0), \
('Drömhus', '', 'http://96.31.83.86:8200/', '', 'Dromhus.png', 0, 0, 0, 1, '2011-11-11', 12064, 0, '2017-01-01 00:00:00', 0), \
('Garth-Brooks', '', 'http://96.31.83.86:8200/', '', 'Garth-Brooks.png', 0, 0, 0, 1, '2011-11-11', 12065, 0, '2017-01-01 00:00:00', 0), \
('Channel-1049', '', 'http://96.31.83.86:8200/', 'http://www.wkos.com/', 'Channel-1049.png', 0, 0, 0, 1, '2011-11-11', 12066, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-970', '', 'http://96.31.83.86:8200/', 'http://www.foxsports970am.com/', 'Fox-Sports-970.png', 0, 0, 0, 1, '2011-11-11', 12067, 0, '2017-01-01 00:00:00', 0), \
('Fabulous-1580', '', 'http://96.31.83.86:8200/', 'http://www.kchaam.com/', 'Fabulous-1580.png', 0, 0, 0, 1, '2011-11-11', 12068, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-1670', '', 'http://96.31.83.86:8200/', 'http://www.foxsports1670.com/', 'Fox-Sports-1670.png', 0, 0, 0, 1, '2011-11-11', 12069, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Dead', 'We dig deeply into personal collections, various hidden stashes, and official releases to create a station that makes any Deadhead&#39;s dream come true and keeps it alive.', 'http://96.31.83.86:8200/', 'http://www.radioio.com/channels/dead', 'RadioIO-Dead.png', 0, 0, 0, 1, '2011-11-11', 12070, 0, '2017-01-01 00:00:00', 0), \
('ESPN-GameDay', '', 'http://96.31.83.86:8200/', '', 'ESPN-GameDay.png', 0, 0, 0, 1, '2011-11-11', 12071, 0, '2017-01-01 00:00:00', 0), \
('Meredith-Andrews', '', 'http://96.31.83.86:8200/', '', 'Meredith-Andrews.png', 0, 0, 0, 1, '2011-11-11', 12072, 0, '2017-01-01 00:00:00', 0), \
('Smooth-Jazz-Sunday-Morning', '', 'http://96.31.83.86:8200/', '', 'Smooth-Jazz-Sunday-Morning.png', 0, 0, 0, 1, '2011-11-11', 12073, 0, '2017-01-01 00:00:00', 0), \
('WREK-911', 'Georgia Tech&#39;s Student run radio station.', 'http://96.31.83.86:8200/', 'http://www.wrek.org/', 'WREK-911.png', 0, 0, 0, 1, '2011-11-11', 12074, 0, '2017-01-01 00:00:00', 0), \
('Carpark-North', '', 'http://96.31.83.86:8200/', '', 'Carpark-North.png', 0, 0, 0, 1, '2011-11-11', 12075, 0, '2017-01-01 00:00:00', 0), \
('The-Sunday-Cafe', '', 'http://96.31.83.86:8200/', '', 'The-Sunday-Cafe.png', 0, 0, 0, 1, '2011-11-11', 12076, 0, '2017-01-01 00:00:00', 0), \
('Lars-Peterson', '', 'http://96.31.83.86:8200/', '', 'Lars-Peterson.png', 0, 0, 0, 1, '2011-11-11', 12077, 0, '2017-01-01 00:00:00', 0), \
('Trance-and-dance', 'Trance, dance, techno, house, clubbing 24h/24 ! Enjoy !', 'http://96.31.83.86:8200/', 'http://www.trance-and-dance.com/', 'Trance-and-dance.png', 0, 0, 0, 1, '2011-11-11', 12078, 0, '2017-01-01 00:00:00', 0), \
('Mike-Lupica', '', 'http://96.31.83.86:8200/', '', 'Mike-Lupica.png', 0, 0, 0, 1, '2011-11-11', 12079, 0, '2017-01-01 00:00:00', 0), \
('Spin-Doctors', '', 'http://96.31.83.86:8200/', '', 'Spin-Doctors.png', 0, 0, 0, 1, '2011-11-11', 12080, 0, '2017-01-01 00:00:00', 0), \
('WRUW-FM-911', '', 'http://96.31.83.86:8200/', 'http://www.wruw.org/', 'WRUW-FM-911.png', 0, 0, 0, 1, '2011-11-11', 12081, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1320', '1320 KFAN is the exclusive home of the Utah Jazz, as well as live local talk about Utah&#39;s biggest sports news. KFNZ also features play-by-play for the NFL, the NBA and MLB. 1320 KFAN has extensive', 'http://96.31.83.86:8200/', 'http://www.thefansports.com/', 'The-Fan-1320.png', 0, 0, 0, 1, '2011-11-11', 12082, 0, '2017-01-01 00:00:00', 0), \
('Mario', '', 'http://96.31.83.86:8200/', '', 'Mario.png', 0, 0, 0, 1, '2011-11-11', 12083, 0, '2017-01-01 00:00:00', 0), \
('Pop-Schlager-Radio', '', 'http://96.31.83.86:8200/', 'http://www.popschlagerradio.com/', 'Pop-Schlager-Radio.png', 0, 0, 0, 1, '2011-11-11', 12084, 0, '2017-01-01 00:00:00', 0), \
('Radio-RMF-Blues', 'RMFOn.pl to kilkadziesiat tematycznych stacji radiowych, dedykowanych gatunkom, dekadom, krajom a nawet artystom. Wybierz radio internetowe i znajdz muzyke dla siebie.', 'http://96.31.83.86:8200/', 'http://www.miastomuzyki.pl/', 'Radio-RMF-Blues.png', 0, 0, 0, 1, '2011-11-11', 12085, 0, '2017-01-01 00:00:00', 0), \
('WHLL-Sports-Talk', '', 'http://96.31.83.86:8200/', '', 'WHLL-Sports-Talk.png', 0, 0, 0, 1, '2011-11-11', 12086, 0, '2017-01-01 00:00:00', 0), \
('929-the-River', '', 'http://96.31.83.86:8200/', 'http://www.929theriver.com', '929-the-River.png', 0, 0, 0, 1, '2011-11-11', 12087, 0, '2017-01-01 00:00:00', 0), \
('WBAT-1400', '', 'http://96.31.83.86:8200/', 'http://www.wbat.com/', 'WBAT-1400.png', 0, 0, 0, 1, '2011-11-11', 12088, 0, '2017-01-01 00:00:00', 0), \
('Q923', '', 'http://96.31.83.86:8200/', 'http://www.q923.net/', 'Q923.png', 0, 0, 0, 1, '2011-11-11', 12089, 0, '2017-01-01 00:00:00', 0), \
('Kyle-Eastwood', '', 'http://96.31.83.86:8200/', '', 'Kyle-Eastwood.png', 0, 0, 0, 1, '2011-11-11', 12090, 0, '2017-01-01 00:00:00', 0), \
('Moving-Forward', '', 'http://96.31.83.86:8200/', '', 'Moving-Forward.png', 0, 0, 0, 1, '2011-11-11', 12091, 0, '2017-01-01 00:00:00', 0), \
('California-101', '', 'http://96.31.83.86:8200/', 'http://www.california101radio.com/', 'California-101.png', 0, 0, 0, 1, '2011-11-11', 12092, 0, '2017-01-01 00:00:00', 0), \
('Raute-MusikFM-Funky', '', 'http://96.31.83.86:8200/', 'http://www.rautemusik.fm/', 'Raute-MusikFM-Funky.png', 0, 0, 0, 1, '2011-11-11', 12093, 0, '2017-01-01 00:00:00', 0), \
('Listen-Up', '', 'http://96.31.83.86:8200/', '', 'Listen-Up.png', 0, 0, 0, 1, '2011-11-11', 12094, 0, '2017-01-01 00:00:00', 0), \
('Soft-Cell', '', 'http://96.31.83.86:8200/', '', 'Soft-Cell.png', 0, 0, 0, 1, '2011-11-11', 12095, 0, '2017-01-01 00:00:00', 0), \
('Capricorn-FM-960', '', 'http://96.31.83.86:8200/', 'http://www.capricornfm.co.za/', 'Capricorn-FM-960.png', 0, 0, 0, 1, '2011-11-11', 12096, 0, '2017-01-01 00:00:00', 0), \
('WHCU-870', '', 'http://96.31.83.86:8200/', 'http://www.whcu870.com/', 'WHCU-870.png', 0, 0, 0, 1, '2011-11-11', 12097, 0, '2017-01-01 00:00:00', 0), \
('Rock-94-12-945', '', 'http://96.31.83.86:8200/', 'http://rock945.com/', 'Rock-94-12-945.png', 0, 0, 0, 1, '2011-11-11', 12098, 0, '2017-01-01 00:00:00', 0), \
('Sunrise-Radio-1458', 'Sunrise Radio is the UKs biggest Asian radio station, on-air to 2 million weekly listeners on satellite, cable, DAB digital radio and mediumwave.', 'http://96.31.83.86:8200/', 'http://www.sunriseradio.com/', 'Sunrise-Radio-1458.png', 0, 0, 0, 1, '2011-11-11', 12099, 0, '2017-01-01 00:00:00', 0), \
('The-Sports-Club', '', 'http://96.31.83.86:8200/', '', 'The-Sports-Club.png', 0, 0, 0, 1, '2011-11-11', 12100, 0, '2017-01-01 00:00:00', 0), \
('WRNX-1009', '', 'http://96.31.83.86:8200/', 'http://www.wrnx.com/', 'WRNX-1009.png', 0, 0, 0, 1, '2011-11-11', 12101, 0, '2017-01-01 00:00:00', 0), \
('Frankie-Smith', '', 'http://96.31.83.86:8200/', '', 'Frankie-Smith.png', 0, 0, 0, 1, '2011-11-11', 12102, 0, '2017-01-01 00:00:00', 0), \
('International-Cricket', '', 'http://96.31.83.86:8200/', '', 'International-Cricket.png', 0, 0, 0, 1, '2011-11-11', 12103, 0, '2017-01-01 00:00:00', 0), \
('Jaffa', '', 'http://96.31.83.86:8200/', '', 'Jaffa.png', 0, 0, 0, 1, '2011-11-11', 12104, 0, '2017-01-01 00:00:00', 0), \
('Creedence-Clearwater-Revival', '', 'http://96.31.83.86:8200/', '', 'Creedence-Clearwater-Revival.png', 0, 0, 0, 1, '2011-11-11', 12105, 0, '2017-01-01 00:00:00', 0), \
('Styx', '', 'http://96.31.83.86:8200/', '', 'Styx.png', 0, 0, 0, 1, '2011-11-11', 12106, 0, '2017-01-01 00:00:00', 0), \
('VFX-945', '', 'http://96.31.83.86:8200/', 'http://www.utahsvfx.com/', 'VFX-945.png', 0, 0, 0, 1, '2011-11-11', 12107, 0, '2017-01-01 00:00:00', 0), \
('917xfm-917', 'Hier finden Musikstile ihren Platz, die es im Radio sonst eher schwer haben, wie Indie, Alternative, Elektro oder Jazz. Ein besonderer Schwerpunkt liegt auf der Hamburger Musikszene und zwischen 19 un', 'http://96.31.83.86:8200/', 'http://www.917xfm.de/', '917xfm-917.png', 0, 0, 0, 1, '2011-11-11', 12108, 0, '2017-01-01 00:00:00', 0), \
('Denise-Donatelli', '', 'http://96.31.83.86:8200/', '', 'Denise-Donatelli.png', 0, 0, 0, 1, '2011-11-11', 12109, 0, '2017-01-01 00:00:00', 0), \
('Kool-1280', '', 'http://96.31.83.86:8200/', 'http://www.kool1023.com', 'Kool-1280.png', 0, 0, 0, 1, '2011-11-11', 12110, 0, '2017-01-01 00:00:00', 0), \
('Country-96-957', '', 'http://96.31.83.86:8200/', 'http://info.kwwr.com/', 'Country-96-957.png', 0, 0, 0, 1, '2011-11-11', 12111, 0, '2017-01-01 00:00:00', 0), \
('Sweet', '', 'http://96.31.83.86:8200/', '', 'Sweet.png', 0, 0, 0, 1, '2011-11-11', 12112, 0, '2017-01-01 00:00:00', 0), \
('La-Gigante-1330', '', 'http://96.31.83.86:8200/', 'http://lagigante1330.com/', 'La-Gigante-1330.png', 0, 0, 0, 1, '2011-11-11', 12113, 0, '2017-01-01 00:00:00', 0), \
('Rolf-Zuckowski', '', 'http://96.31.83.86:8200/', '', 'Rolf-Zuckowski.png', 0, 0, 0, 1, '2011-11-11', 12114, 0, '2017-01-01 00:00:00', 0), \
('Rock-FM-945', '', 'http://96.31.83.86:8200/', 'http://www.rockfm.com.tr/', 'Rock-FM-945.png', 0, 0, 0, 1, '2011-11-11', 12115, 0, '2017-01-01 00:00:00', 0), \
('Pissing-Razors', '', 'http://96.31.83.86:8200/', '', 'Pissing-Razors.png', 0, 0, 0, 1, '2011-11-11', 12116, 0, '2017-01-01 00:00:00', 0), \
('KMSR-1520', '', 'http://96.31.83.86:8200/', 'http://www.kmsrradio.com', 'KMSR-1520.png', 0, 0, 0, 1, '2011-11-11', 12117, 0, '2017-01-01 00:00:00', 0), \
('Waldeck', '', 'http://96.31.83.86:8200/', '', 'Waldeck.png', 0, 0, 0, 1, '2011-11-11', 12118, 0, '2017-01-01 00:00:00', 0), \
('WQMU-925', '', 'http://96.31.83.86:8200/', 'http://www.u92radio.com/', 'WQMU-925.png', 0, 0, 0, 1, '2011-11-11', 12119, 0, '2017-01-01 00:00:00', 0), \
('KAST-1370', '', 'http://96.31.83.86:8200/', 'http://www.kast1370.com/', 'KAST-1370.png', 0, 0, 0, 1, '2011-11-11', 12120, 0, '2017-01-01 00:00:00', 0), \
('K-Rock-1007', '', 'http://96.31.83.86:8200/', 'http://www.maddog.net/', 'K-Rock-1007.png', 0, 0, 0, 1, '2011-11-11', 12121, 0, '2017-01-01 00:00:00', 0), \
('Talk-Radio-1210-WPHT', 'More Live.  More Local.', 'http://96.31.83.86:8200/', 'http://www.cbsphilly.com', 'Talk-Radio-1210-WPHT.png', 0, 0, 0, 1, '2011-11-11', 12122, 0, '2017-01-01 00:00:00', 0), \
('Grant-Geissman', '', 'http://96.31.83.86:8200/', '', 'Grant-Geissman.png', 0, 0, 0, 1, '2011-11-11', 12123, 0, '2017-01-01 00:00:00', 0), \
('Liquido', '', 'http://96.31.83.86:8200/', '', 'Liquido.png', 0, 0, 0, 1, '2011-11-11', 12125, 0, '2017-01-01 00:00:00', 0), \
('WOXO-FM-927', '', 'http://96.31.83.86:8200/', 'http://www.woxo.com/', 'WOXO-FM-927.png', 0, 0, 0, 1, '2011-11-11', 12126, 0, '2017-01-01 00:00:00', 0), \
('Eddie-Money', '', 'http://96.31.83.86:8200/', '', 'Eddie-Money.png', 0, 0, 0, 1, '2011-11-11', 12127, 0, '2017-01-01 00:00:00', 0), \
('KZQQ-1560', '', 'http://96.31.83.86:8200/', 'http://www.radioabilene.com/', 'KZQQ-1560.png', 0, 0, 0, 1, '2011-11-11', 12128, 0, '2017-01-01 00:00:00', 0), \
('WEVA-860', '', 'http://96.31.83.86:8200/', 'http://www.wevaradio.com/', 'WEVA-860.png', 0, 0, 0, 1, '2011-11-11', 12129, 0, '2017-01-01 00:00:00', 0), \
('MMA-Junkie-Radio-Podcast', '', 'http://96.31.83.86:8200/', '', 'MMA-Junkie-Radio-Podcast.png', 0, 0, 0, 1, '2011-11-11', 12130, 0, '2017-01-01 00:00:00', 0), \
('ILC-Tamil-Radio', '', 'http://96.31.83.86:8200/', 'http://www.ilctamil.co.uk/', 'ILC-Tamil-Radio.png', 0, 0, 0, 1, '2011-11-11', 12131, 0, '2017-01-01 00:00:00', 0), \
('The-Broadband-Comedy-Network', '', 'http://96.31.83.86:8200/', 'http://www.bcnonline.com/', 'The-Broadband-Comedy-Network.png', 0, 0, 0, 1, '2011-11-11', 12132, 0, '2017-01-01 00:00:00', 0), \
('WTKK-969', '', 'http://96.31.83.86:8200/', 'http://www.969bostontalks.com/', 'WTKK-969.png', 0, 0, 0, 1, '2011-11-11', 12133, 0, '2017-01-01 00:00:00', 0), \
('The-Game', '', 'http://96.31.83.86:8200/', '', 'The-Game.png', 0, 0, 0, 1, '2011-11-11', 12134, 0, '2017-01-01 00:00:00', 0), \
('KODI-1400', '', 'http://96.31.83.86:8200/', 'http://www.bighornradio.com/', 'KODI-1400.png', 0, 0, 0, 1, '2011-11-11', 12135, 0, '2017-01-01 00:00:00', 0), \
('WVSA-1380', '', 'http://96.31.83.86:8200/', 'http://www.wvsa1380.com', 'WVSA-1380.png', 0, 0, 0, 1, '2011-11-11', 12136, 0, '2017-01-01 00:00:00', 0), \
('Monday-Night-Countdown', '', 'http://96.31.83.86:8200/', '', 'Monday-Night-Countdown.png', 0, 0, 0, 1, '2011-11-11', 12137, 0, '2017-01-01 00:00:00', 0), \
('Columbus-Blue-Jackets-Post-Game', '', 'http://96.31.83.86:8200/', '', 'Columbus-Blue-Jackets-Post-Game.png', 0, 0, 0, 1, '2011-11-11', 12138, 0, '2017-01-01 00:00:00', 0), \
('WCEI-FM-967', '', 'http://96.31.83.86:8200/', 'http://www.wceiradio.com/', 'WCEI-FM-967.png', 0, 0, 0, 1, '2011-11-11', 12139, 0, '2017-01-01 00:00:00', 0), \
('The-Takeaway', '', 'http://96.31.83.86:8200/', '', 'The-Takeaway.png', 0, 0, 0, 1, '2011-11-11', 12140, 0, '2017-01-01 00:00:00', 0), \
('Worldwide-Smash', '', 'http://96.31.83.86:8200/', '', 'Worldwide-Smash.png', 0, 0, 0, 1, '2011-11-11', 12141, 0, '2017-01-01 00:00:00', 0), \
('KHOW-Marketplace', '', 'http://96.31.83.86:8200/', '', 'KHOW-Marketplace.png', 0, 0, 0, 1, '2011-11-11', 12142, 0, '2017-01-01 00:00:00', 0), \
('ep-RaDiO-ShoW', 'e..p RaDiO Show &#39;The Voice OF The e..p People&#39; Webradio With Live Worldwide DJ Shows Hip Hop,<BR>Rap,Soul,Funk,Reggae,Dancehall 24/7/365 And Live Request', 'http://96.31.83.86:8200/', 'http://dirtyskeme.free.fr/', 'ep-RaDiO-ShoW.png', 0, 0, 0, 1, '2011-11-11', 12143, 0, '2017-01-01 00:00:00', 0), \
('Magic-1073', '', 'http://96.31.83.86:8200/', 'http://www.iammagic1073.com/', 'Magic-1073.png', 0, 0, 0, 1, '2011-11-11', 12144, 0, '2017-01-01 00:00:00', 0), \
('Petros-and-Money', '', 'http://96.31.83.86:8200/', '', 'Petros-and-Money.png', 0, 0, 0, 1, '2011-11-11', 12145, 0, '2017-01-01 00:00:00', 0), \
('Paddy-Kelly', '', 'http://96.31.83.86:8200/', '', 'Paddy-Kelly.png', 0, 0, 0, 1, '2011-11-11', 12146, 0, '2017-01-01 00:00:00', 0), \
('Matchbox-Twenty', '', 'http://96.31.83.86:8200/', '', 'Matchbox-Twenty.png', 0, 0, 0, 1, '2011-11-11', 12147, 0, '2017-01-01 00:00:00', 0), \
('Austin360-Radio', '', 'http://96.31.83.86:8200/', 'http://www.austin360radio.com', 'Austin360-Radio.png', 0, 0, 0, 1, '2011-11-11', 12148, 0, '2017-01-01 00:00:00', 0), \
('WPAX-1240', '', 'http://96.31.83.86:8200/', 'http://www.wpaxradio.com/', 'WPAX-1240.png', 0, 0, 0, 1, '2011-11-11', 12149, 0, '2017-01-01 00:00:00', 0), \
('Bakersfield-Blaze-Baseball-Network', '', 'http://96.31.83.86:8200/', '', 'Bakersfield-Blaze-Baseball-Network.png', 0, 0, 0, 1, '2011-11-11', 12150, 0, '2017-01-01 00:00:00', 0), \
('Q107-1073', 'Q107 Calgary, Alberta plays the greatest rock &#39;n roll ever made and promises to be unlike any classic rock station in North America. It puts the music first and plays more than just the hits, feat', 'http://96.31.83.86:8200/', 'http://www.q107fm.ca/', 'Q107-1073.png', 0, 0, 0, 1, '2011-11-11', 12151, 0, '2017-01-01 00:00:00', 0), \
('Paul-Anka', '', 'http://96.31.83.86:8200/', '', 'Paul-Anka.png', 0, 0, 0, 1, '2011-11-11', 12152, 0, '2017-01-01 00:00:00', 0), \
('GotRadio-Top-40', 'The freshest blend of today&#39;s Top hits', 'http://96.31.83.86:8200/', 'http://www.gotradio.com/', 'GotRadio-Top-40.png', 0, 0, 0, 1, '2011-11-11', 12154, 0, '2017-01-01 00:00:00', 0), \
('WVWI-Religious-Programming', '', 'http://96.31.83.86:8200/', '', 'WVWI-Religious-Programming.png', 0, 0, 0, 1, '2011-11-11', 12155, 0, '2017-01-01 00:00:00', 0), \
('Indians-at-Giants', '', 'http://96.31.83.86:8200/', '', 'Indians-at-Giants.png', 0, 0, 0, 1, '2011-11-11', 12156, 0, '2017-01-01 00:00:00', 0), \
('KK-93-931', '', 'http://96.31.83.86:8200/', 'http://www.kk93.com/', 'KK-93-931.png', 0, 0, 0, 1, '2011-11-11', 12157, 0, '2017-01-01 00:00:00', 0), \
('Nick-Grimshaw-and-Annie-Mac', '', 'http://96.31.83.86:8200/', '', 'Nick-Grimshaw-and-Annie-Mac.png', 0, 0, 0, 1, '2011-11-11', 12158, 0, '2017-01-01 00:00:00', 0), \
('Sport-on-BBC-Berkshire', '', 'http://96.31.83.86:8200/', '', 'Sport-on-BBC-Berkshire.png', 0, 0, 0, 1, '2011-11-11', 12159, 0, '2017-01-01 00:00:00', 0), \
('Weekend-Arvos', '', 'http://96.31.83.86:8200/', '', 'Weekend-Arvos.png', 0, 0, 0, 1, '2011-11-11', 12160, 0, '2017-01-01 00:00:00', 0), \
('Jazz-Decades-(KCSM)', '', 'http://96.31.83.86:8200/', '', 'Jazz-Decades-(KCSM).png', 0, 0, 0, 1, '2011-11-11', 12161, 0, '2017-01-01 00:00:00', 0), \
('KWRT-1370', '', 'http://96.31.83.86:8200/', 'http://www.1370kwrt.com', 'KWRT-1370.png', 0, 0, 0, 1, '2011-11-11', 12162, 0, '2017-01-01 00:00:00', 0), \
('Illdisposed', '', 'http://96.31.83.86:8200/', '', 'Illdisposed.png', 0, 0, 0, 1, '2011-11-11', 12163, 0, '2017-01-01 00:00:00', 0), \
('Mercyless', '', 'http://96.31.83.86:8200/', '', 'Mercyless.png', 0, 0, 0, 1, '2011-11-11', 12164, 0, '2017-01-01 00:00:00', 0), \
('The-Golf-Show', '', 'http://96.31.83.86:8200/', '', 'The-Golf-Show.png', 0, 0, 0, 1, '2011-11-11', 12165, 0, '2017-01-01 00:00:00', 0), \
('1CLUBFM--Bar-Rockin-Country', 'Uptempo Country dance', 'http://96.31.83.86:8200/', 'http://www.1club.fm/', '1CLUBFM--Bar-Rockin-Country.png', 0, 0, 0, 1, '2011-11-11', 12166, 0, '2017-01-01 00:00:00', 0), \
('Z93-Jamz-933', '', 'http://96.31.83.86:8200/', 'http://www.z93jamz.com/', 'Z93-Jamz-933.png', 0, 0, 0, 1, '2011-11-11', 12167, 0, '2017-01-01 00:00:00', 0), \
('Barbara-Windsors-Funny-Girls', '', 'http://96.31.83.86:8200/', '', 'Barbara-Windsors-Funny-Girls.png', 0, 0, 0, 1, '2011-11-11', 12168, 0, '2017-01-01 00:00:00', 0), \
('WUSY-1007', '', 'http://96.31.83.86:8200/', 'http://www.us101country.com/', 'WUSY-1007.png', 0, 0, 0, 1, '2011-11-11', 12169, 0, '2017-01-01 00:00:00', 0), \
('WRCI-1520', '', 'http://96.31.83.86:8200/', 'http://www.wlkm.com/', 'WRCI-1520.png', 0, 0, 0, 1, '2011-11-11', 12170, 0, '2017-01-01 00:00:00', 0), \
('Christmas-Radio-Network', 'The Christmas Radio Network is styled after the old Top 40 radio format. We play the best from all genres. Our library is quite diverse and features over 1000 songs. From Bing Crosby and Andy Williams', 'http://96.31.83.86:8200/', 'http://www.christmasradionetwork.com/', 'Christmas-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 12171, 0, '2017-01-01 00:00:00', 0), \
('Willie-Nelson', '', 'http://96.31.83.86:8200/', '', 'Willie-Nelson.png', 0, 0, 0, 1, '2011-11-11', 12172, 0, '2017-01-01 00:00:00', 0), \
('Frekvence-1-1025', '', 'http://96.31.83.86:8200/', 'http://www.frekvence1.cz/', 'Frekvence-1-1025.png', 0, 0, 0, 1, '2011-11-11', 12173, 0, '2017-01-01 00:00:00', 0), \
('Classic-1280', '', 'http://96.31.83.86:8200/', 'http://www.kdkd.net', 'Classic-1280.png', 0, 0, 0, 1, '2011-11-11', 12174, 0, '2017-01-01 00:00:00', 0), \
('Laurent-Wolf', '', 'http://96.31.83.86:8200/', '', 'Laurent-Wolf.png', 0, 0, 0, 1, '2011-11-11', 12175, 0, '2017-01-01 00:00:00', 0), \
('WOC-1420', '', 'http://96.31.83.86:8200/', 'http://www.woc1420.com/', 'WOC-1420.png', 0, 0, 0, 1, '2011-11-11', 12176, 0, '2017-01-01 00:00:00', 0), \
('Somali-Radio-889', '', 'http://96.31.83.86:8200/', 'http://www.somaliradio.dk/', 'Somali-Radio-889.png', 0, 0, 0, 1, '2011-11-11', 12177, 0, '2017-01-01 00:00:00', 0), \
('Stomp-Radio', 'Stomp Radio broadcasts soul, funk, disco and jazz 24/7 from London, United kingdom.', 'http://96.31.83.86:8200/', 'http://www.stompradio.com/', 'Stomp-Radio.png', 0, 0, 0, 1, '2011-11-11', 12178, 0, '2017-01-01 00:00:00', 0), \
('The-Mighty-Mighty-Bosstones', '', 'http://96.31.83.86:8200/', '', 'The-Mighty-Mighty-Bosstones.png', 0, 0, 0, 1, '2011-11-11', 12179, 0, '2017-01-01 00:00:00', 0), \
('Clap-Your-Hands-Say-Yeah', '', 'http://96.31.83.86:8200/', '', 'Clap-Your-Hands-Say-Yeah.png', 0, 0, 0, 1, '2011-11-11', 12180, 0, '2017-01-01 00:00:00', 0), \
('1080FM-Top-40', '', 'http://96.31.83.86:8200/', 'http://www.1080.fm/', '1080FM-Top-40.png', 0, 0, 0, 1, '2011-11-11', 12181, 0, '2017-01-01 00:00:00', 0), \
('Krautland-Radio', '', 'http://96.31.83.86:8200/', 'http://www.laut.fm/krautland', 'Krautland-Radio.png', 0, 0, 0, 1, '2011-11-11', 12182, 0, '2017-01-01 00:00:00', 0), \
('979-The-River', '#1 At-Work station with Steve Keller hosting mornings, long sets of Light Rock every hour, the River 9 to 5 No-Repeat Workday.', 'http://96.31.83.86:8200/', 'http://www.kvvr.com/', '979-The-River.png', 0, 0, 0, 1, '2011-11-11', 12183, 0, '2017-01-01 00:00:00', 0), \
('The-Coasters', '', 'http://96.31.83.86:8200/', '', 'The-Coasters.png', 0, 0, 0, 1, '2011-11-11', 12184, 0, '2017-01-01 00:00:00', 0), \
('ESPN-990', '', 'http://96.31.83.86:8200/', 'http://www.espn990.com/', 'ESPN-990.png', 0, 0, 0, 1, '2011-11-11', 12185, 0, '2017-01-01 00:00:00', 0), \
('The-System', '', 'http://96.31.83.86:8200/', '', 'The-System.png', 0, 0, 0, 1, '2011-11-11', 12186, 0, '2017-01-01 00:00:00', 0), \
('Iyaz', '', 'http://96.31.83.86:8200/', '', 'Iyaz.png', 0, 0, 0, 1, '2011-11-11', 12187, 0, '2017-01-01 00:00:00', 0), \
('WIEL-1400', '', 'http://96.31.83.86:8200/', '', 'WIEL-1400.png', 0, 0, 0, 1, '2011-11-11', 12188, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-1360', '', 'http://96.31.83.86:8200/', 'http://www.foxsports1360.com/', 'Fox-Sports-1360.png', 0, 0, 0, 1, '2011-11-11', 12189, 0, '2017-01-01 00:00:00', 0), \
('idobi-Radio', 'Idobi Network: Music news, CD Reviews, artist interviews, and internet radio', 'http://96.31.83.86:8200/', 'http://idobi.com/radio/', 'idobi-Radio.png', 0, 0, 0, 1, '2011-11-11', 12190, 0, '2017-01-01 00:00:00', 0), \
('KRSN-1490', '', 'http://96.31.83.86:8200/', 'http://www.krsnam1490.com/', 'KRSN-1490.png', 0, 0, 0, 1, '2011-11-11', 12191, 0, '2017-01-01 00:00:00', 0), \
('The-Savage-Dog-Show-Podcast', '', 'http://96.31.83.86:8200/', '', 'The-Savage-Dog-Show-Podcast.png', 0, 0, 0, 1, '2011-11-11', 12192, 0, '2017-01-01 00:00:00', 0), \
('Devon-Williams', '', 'http://96.31.83.86:8200/', '', 'Devon-Williams.png', 0, 0, 0, 1, '2011-11-11', 12193, 0, '2017-01-01 00:00:00', 0), \
('The-Mills-Brothers', '', 'http://96.31.83.86:8200/', '', 'The-Mills-Brothers.png', 0, 0, 0, 1, '2011-11-11', 12194, 0, '2017-01-01 00:00:00', 0), \
('Benji-B', '', 'http://96.31.83.86:8200/', '', 'Benji-B.png', 0, 0, 0, 1, '2011-11-11', 12195, 0, '2017-01-01 00:00:00', 0), \
('Bullet-for-My-Valentine', '', 'http://96.31.83.86:8200/', '', 'Bullet-for-My-Valentine.png', 0, 0, 0, 1, '2011-11-11', 12196, 0, '2017-01-01 00:00:00', 0), \
('LoCash-Cowboys', '', 'http://96.31.83.86:8200/', '', 'LoCash-Cowboys.png', 0, 0, 0, 1, '2011-11-11', 12197, 0, '2017-01-01 00:00:00', 0), \
('WGAC-580', '', 'http://96.31.83.86:8200/', 'http://www.wgac.com/', 'WGAC-580.png', 0, 0, 0, 1, '2011-11-11', 12198, 0, '2017-01-01 00:00:00', 0), \
('George-Michael', '', 'http://96.31.83.86:8200/', '', 'George-Michael.png', 0, 0, 0, 1, '2011-11-11', 12199, 0, '2017-01-01 00:00:00', 0), \
('DVFM-Lounge', 'DV.FM - The Sound of Style, is a non profit, non commercial luxury radio streaming a 24 hours service to your home and business. Only the finest in music selected for your enjoyment and pleasure which', 'http://96.31.83.86:8200/', 'http://www.dv.fm/v4/', 'DVFM-Lounge.png', 0, 0, 0, 1, '2011-11-11', 12200, 0, '2017-01-01 00:00:00', 0), \
('Warm-98-985', '', 'http://96.31.83.86:8200/', 'http://www.warm98.com/', 'Warm-98-985.png', 0, 0, 0, 1, '2011-11-11', 12201, 0, '2017-01-01 00:00:00', 0), \
('Mellow-Jazz-on-JAZZRADIOcom', 'Our special mix of jazz, from classics to contemporary.', 'http://96.31.83.86:8200/', 'http://www.jazzradio.com/', 'Mellow-Jazz-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 12202, 0, '2017-01-01 00:00:00', 0), \
('Gossip', '', 'http://96.31.83.86:8200/', '', 'Gossip.png', 0, 0, 0, 1, '2011-11-11', 12203, 0, '2017-01-01 00:00:00', 0), \
('Moth', '', 'http://96.31.83.86:8200/', '', 'Moth.png', 0, 0, 0, 1, '2011-11-11', 12204, 0, '2017-01-01 00:00:00', 0), \
('WGCL-1370', '', 'http://96.31.83.86:8200/', 'http://www.wgclradio.com/', 'WGCL-1370.png', 0, 0, 0, 1, '2011-11-11', 12205, 0, '2017-01-01 00:00:00', 0), \
('WVUM-905', '', 'http://96.31.83.86:8200/', 'http://www.wvum.org/', 'WVUM-905.png', 0, 0, 0, 1, '2011-11-11', 12206, 0, '2017-01-01 00:00:00', 0), \
('WCAB-590', '', 'http://96.31.83.86:8200/', 'http://www.wcab59.com/main.php', 'WCAB-590.png', 0, 0, 0, 1, '2011-11-11', 12207, 0, '2017-01-01 00:00:00', 0), \
('Yellowcard', '', 'http://96.31.83.86:8200/', '', 'Yellowcard.png', 0, 0, 0, 1, '2011-11-11', 12208, 0, '2017-01-01 00:00:00', 0), \
('WKOR-980', '', 'http://96.31.83.86:8200/', '', 'WKOR-980.png', 0, 0, 0, 1, '2011-11-11', 12209, 0, '2017-01-01 00:00:00', 0), \
('Jody-Watley', '', 'http://96.31.83.86:8200/', '', 'Jody-Watley.png', 0, 0, 0, 1, '2011-11-11', 12210, 0, '2017-01-01 00:00:00', 0), \
('KAVL-610', '', 'http://96.31.83.86:8200/', 'http://www.foxsports610.com/', 'KAVL-610.png', 0, 0, 0, 1, '2011-11-11', 12211, 0, '2017-01-01 00:00:00', 0), \
('KTSA-550', '', 'http://96.31.83.86:8200/', 'http://www.ktsa.com/', 'KTSA-550.png', 0, 0, 0, 1, '2011-11-11', 12212, 0, '2017-01-01 00:00:00', 0), \
('Country-1045', '', 'http://96.31.83.86:8200/', 'http://www.1045wsld.com/', 'Country-1045.png', 0, 0, 0, 1, '2011-11-11', 12213, 0, '2017-01-01 00:00:00', 0), \
('Positively-Wall-Street', '', 'http://96.31.83.86:8200/', '', 'Positively-Wall-Street.png', 0, 0, 0, 1, '2011-11-11', 12214, 0, '2017-01-01 00:00:00', 0), \
('Sash', '', 'http://96.31.83.86:8200/', '', 'Sash.png', 0, 0, 0, 1, '2011-11-11', 12216, 0, '2017-01-01 00:00:00', 0), \
('Thompson-Square', '', 'http://96.31.83.86:8200/', '', 'Thompson-Square.png', 0, 0, 0, 1, '2011-11-11', 12217, 0, '2017-01-01 00:00:00', 0), \
('KAAT-1031', '', 'http://96.31.83.86:8200/', 'http://www.kaat.com/', 'KAAT-1031.png', 0, 0, 0, 1, '2011-11-11', 12218, 0, '2017-01-01 00:00:00', 0), \
('omg-Radio-1067', '', 'http://96.31.83.86:8200/', 'http://www.1067omgradio.com', 'omg-Radio-1067.png', 0, 0, 0, 1, '2011-11-11', 12219, 0, '2017-01-01 00:00:00', 0), \
('De-Phazz', '', 'http://96.31.83.86:8200/', '', 'De-Phazz.png', 0, 0, 0, 1, '2011-11-11', 12220, 0, '2017-01-01 00:00:00', 0), \
('Stevie-Wonder', '', 'http://96.31.83.86:8200/', '', 'Stevie-Wonder.png', 0, 0, 0, 1, '2011-11-11', 12221, 0, '2017-01-01 00:00:00', 0), \
('Club-FM-1004', '', 'http://96.31.83.86:8200/', 'http://www.clubfm.net/', 'Club-FM-1004.png', 0, 0, 0, 1, '2011-11-11', 12222, 0, '2017-01-01 00:00:00', 0), \
('Tele-Capri-Sport', 'Aggiornamenti sugli sport campani ed in primo piano il calcio Napoli', 'http://96.31.83.86:8200/', 'http://www.telecaprisport.it/', 'Tele-Capri-Sport.png', 0, 0, 0, 1, '2011-11-11', 12223, 0, '2017-01-01 00:00:00', 0), \
('Lietus-Radio-Vilnius-1059', '', 'http://96.31.83.86:8200/', 'http://www.m-1.fm/', 'Lietus-Radio-Vilnius-1059.png', 0, 0, 0, 1, '2011-11-11', 12224, 0, '2017-01-01 00:00:00', 0), \
('NASCAR-News', '', 'http://96.31.83.86:8200/', '', 'NASCAR-News.png', 0, 0, 0, 1, '2011-11-11', 12225, 0, '2017-01-01 00:00:00', 0), \
('Rosa-Rademakers', '', 'http://96.31.83.86:8200/', '', 'Rosa-Rademakers.png', 0, 0, 0, 1, '2011-11-11', 12226, 0, '2017-01-01 00:00:00', 0), \
('Carosello-Italiano', '', 'http://96.31.83.86:8200/', '', 'Carosello-Italiano.png', 0, 0, 0, 1, '2011-11-11', 12227, 0, '2017-01-01 00:00:00', 0), \
('Darryl-Worley', '', 'http://96.31.83.86:8200/', '', 'Darryl-Worley.png', 0, 0, 0, 1, '2011-11-11', 12228, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1450', '', 'http://96.31.83.86:8200/', '', 'ESPN-1450.png', 0, 0, 0, 1, '2011-11-11', 12229, 0, '2017-01-01 00:00:00', 0), \
('Alabama-Football-Pre-Game', '', 'http://96.31.83.86:8200/', '', 'Alabama-Football-Pre-Game.png', 0, 0, 0, 1, '2011-11-11', 12230, 0, '2017-01-01 00:00:00', 0), \
('Real-Radio-Northeast-1000', '', 'http://96.31.83.86:8200/', 'http://www.realradionortheast.co.uk/', 'Real-Radio-Northeast-1000.png', 0, 0, 0, 1, '2011-11-11', 12231, 0, '2017-01-01 00:00:00', 0), \
('Tee-It-Up-with-Kevin-Haime', '', 'http://96.31.83.86:8200/', '', 'Tee-It-Up-with-Kevin-Haime.png', 0, 0, 0, 1, '2011-11-11', 12232, 0, '2017-01-01 00:00:00', 0), \
('Estelle', '', 'http://96.31.83.86:8200/', '', 'Estelle.png', 0, 0, 0, 1, '2011-11-11', 12233, 0, '2017-01-01 00:00:00', 0), \
('El-Gran-Combo-de-Puerto-Rico', '', 'http://96.31.83.86:8200/', '', 'El-Gran-Combo-de-Puerto-Rico.png', 0, 0, 0, 1, '2011-11-11', 12234, 0, '2017-01-01 00:00:00', 0), \
('Russell-Gunn', '', 'http://96.31.83.86:8200/', '', 'Russell-Gunn.png', 0, 0, 0, 1, '2011-11-11', 12235, 0, '2017-01-01 00:00:00', 0), \
('WLOH-1320', '', 'http://96.31.83.86:8200/', 'http://www.wloh.net/home.html', 'WLOH-1320.png', 0, 0, 0, 1, '2011-11-11', 12236, 0, '2017-01-01 00:00:00', 0), \
('The-Lords-of-the-New-Church', '', 'http://96.31.83.86:8200/', '', 'The-Lords-of-the-New-Church.png', 0, 0, 0, 1, '2011-11-11', 12237, 0, '2017-01-01 00:00:00', 0), \
('With-Jesus-in-everyday-life', '', 'http://96.31.83.86:8200/', '', 'With-Jesus-in-everyday-life.png', 0, 0, 0, 1, '2011-11-11', 12238, 0, '2017-01-01 00:00:00', 0), \
('Tim-McGraw', '', 'http://96.31.83.86:8200/', '', 'Tim-McGraw.png', 0, 0, 0, 1, '2011-11-11', 12239, 0, '2017-01-01 00:00:00', 0), \
('WSB-750', '', 'http://96.31.83.86:8200/', 'http://wsbradio.com/', 'WSB-750.png', 0, 0, 0, 1, '2011-11-11', 12240, 0, '2017-01-01 00:00:00', 0), \
('WXAL-1400', '', 'http://96.31.83.86:8200/', '', 'WXAL-1400.png', 0, 0, 0, 1, '2011-11-11', 12241, 0, '2017-01-01 00:00:00', 0), \
('An-Horse', '', 'http://96.31.83.86:8200/', '', 'An-Horse.png', 0, 0, 0, 1, '2011-11-11', 12242, 0, '2017-01-01 00:00:00', 0), \
('Business550', '', 'http://96.31.83.86:8200/', 'http://www.businessradio550.com', 'Business550.png', 0, 0, 0, 1, '2011-11-11', 12243, 0, '2017-01-01 00:00:00', 0), \
('Star-945', '', 'http://96.31.83.86:8200/', 'http://star94fm.com/', 'Star-945.png', 0, 0, 0, 1, '2011-11-11', 12244, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Night-Jazz-(CJRT)', '', 'http://96.31.83.86:8200/', '', 'Sunday-Night-Jazz-(CJRT).png', 0, 0, 0, 1, '2011-11-11', 12245, 0, '2017-01-01 00:00:00', 0), \
('BBN-English-935', '', 'http://96.31.83.86:8200/', 'http://www.bbnradio.org/wcm4/tabid/3103/Default.aspx', 'BBN-English-935.png', 0, 0, 0, 1, '2011-11-11', 12246, 0, '2017-01-01 00:00:00', 0), \
('WTGA-1590', '', 'http://96.31.83.86:8200/', '', 'WTGA-1590.png', 0, 0, 0, 1, '2011-11-11', 12247, 0, '2017-01-01 00:00:00', 0), \
('Elton-John', '', 'http://96.31.83.86:8200/', '', 'Elton-John.png', 0, 0, 0, 1, '2011-11-11', 12248, 0, '2017-01-01 00:00:00', 0), \
('881-WAY-FM', 'We are North Florida and South Georgia&#39;s 88.1 WAY-FM. Playing non-stop Christian music - all your favorites.', 'http://96.31.83.86:8200/', 'http://wayt.wayfm.com/', '881-WAY-FM.png', 0, 0, 0, 1, '2011-11-11', 12249, 0, '2017-01-01 00:00:00', 0), \
('WAXM-935', 'WAXM 93.5 FM is YOUR Five Star Country Giant, serving the Coalfields of Southwest Virginia &amp; Eastern Tennessee, reaching into five states.Tune in each and everyday to hear our great lineup of mode', 'http://96.31.83.86:8200/', 'http://www.waxm.com/', 'WAXM-935.png', 0, 0, 0, 1, '2011-11-11', 12250, 0, '2017-01-01 00:00:00', 0), \
('HorrorPops', '', 'http://96.31.83.86:8200/', '', 'HorrorPops.png', 0, 0, 0, 1, '2011-11-11', 12251, 0, '2017-01-01 00:00:00', 0), \
('RTS-80s-90s-TODAY', 'La radio online di musica anni &#39;80, &#39;90 e di oggi!', 'http://96.31.83.86:8200/', 'http://rts80s.altervista.org/blog/', 'RTS-80s-90s-TODAY.png', 0, 0, 0, 1, '2011-11-11', 12252, 0, '2017-01-01 00:00:00', 0), \
('WELE-1380', '', 'http://96.31.83.86:8200/', 'http://www.goliathradio.com/index.html', 'WELE-1380.png', 0, 0, 0, 1, '2011-11-11', 12253, 0, '2017-01-01 00:00:00', 0), \
('Mix-967', '', 'http://96.31.83.86:8200/', 'http://www.wbvi.com/', 'Mix-967.png', 0, 0, 0, 1, '2011-11-11', 12254, 0, '2017-01-01 00:00:00', 0), \
('Weekend-Lunch', '', 'http://96.31.83.86:8200/', '', 'Weekend-Lunch.png', 0, 0, 0, 1, '2011-11-11', 12255, 0, '2017-01-01 00:00:00', 0), \
('KPHT-955', '', 'http://96.31.83.86:8200/', 'http://www.kpht955.com/', 'KPHT-955.png', 0, 0, 0, 1, '2011-11-11', 12256, 0, '2017-01-01 00:00:00', 0), \
('WMKT-1270', '', 'http://96.31.83.86:8200/', 'http://www.wmktthetalkstation.com/', 'WMKT-1270.png', 0, 0, 0, 1, '2011-11-11', 12257, 0, '2017-01-01 00:00:00', 0), \
('Memorial-United-Methodist-Church', '', 'http://96.31.83.86:8200/', '', 'Memorial-United-Methodist-Church.png', 0, 0, 0, 1, '2011-11-11', 12258, 0, '2017-01-01 00:00:00', 0), \
('Quaywest-Online', '', 'http://96.31.83.86:8200/', 'http://www.quaywestonline.co.uk', 'Quaywest-Online.png', 0, 0, 0, 1, '2011-11-11', 12259, 0, '2017-01-01 00:00:00', 0), \
('The-Meters', '', 'http://96.31.83.86:8200/', '', 'The-Meters.png', 0, 0, 0, 1, '2011-11-11', 12260, 0, '2017-01-01 00:00:00', 0), \
('MyJam-Radio', 'Playing Your R&amp;B With Soul Jamz And A Touch Of Hip-Hop For The Grown &amp; Sexy With Swagga&#39;', 'http://96.31.83.86:8200/', 'http://www.myjamradio.com', 'MyJam-Radio.png', 0, 0, 0, 1, '2011-11-11', 12261, 0, '2017-01-01 00:00:00', 0), \
('KZSU-901', 'KZSU is Stanford University&#39;s FM radio station, broadcasting across the Bay Area on 90.1 FM and across the world at kzsulive.stanford.edu. We exist to serve the Stanford community with quality rad', 'http://96.31.83.86:8200/', 'http://kzsu.stanford.edu/', 'KZSU-901.png', 0, 0, 0, 1, '2011-11-11', 12262, 0, '2017-01-01 00:00:00', 0), \
('The-Roots', '', 'http://96.31.83.86:8200/', '', 'The-Roots.png', 0, 0, 0, 1, '2011-11-11', 12263, 0, '2017-01-01 00:00:00', 0), \
('Old-Testament', '', 'http://96.31.83.86:8200/', '', 'Old-Testament.png', 0, 0, 0, 1, '2011-11-11', 12264, 0, '2017-01-01 00:00:00', 0), \
('Rock-937', '', 'http://96.31.83.86:8200/', 'http://www.rock937fm.com/', 'Rock-937.png', 0, 0, 0, 1, '2011-11-11', 12265, 0, '2017-01-01 00:00:00', 0), \
('Alex-Lester', '', 'http://96.31.83.86:8200/', '', 'Alex-Lester.png', 0, 0, 0, 1, '2011-11-11', 12266, 0, '2017-01-01 00:00:00', 0), \
('12-Stones', '', 'http://96.31.83.86:8200/', '', '12-Stones.png', 0, 0, 0, 1, '2011-11-11', 12267, 0, '2017-01-01 00:00:00', 0), \
('Greg-Likens', '', 'http://96.31.83.86:8200/', '', 'Greg-Likens.png', 0, 0, 0, 1, '2011-11-11', 12269, 0, '2017-01-01 00:00:00', 0), \
('Obituary', '', 'http://96.31.83.86:8200/', '', 'Obituary.png', 0, 0, 0, 1, '2011-11-11', 12270, 0, '2017-01-01 00:00:00', 0), \
('Byron-Cage', '', 'http://96.31.83.86:8200/', '', 'Byron-Cage.png', 0, 0, 0, 1, '2011-11-11', 12271, 0, '2017-01-01 00:00:00', 0), \
('The-Buzz-989', '', 'http://96.31.83.86:8200/', 'http://www.rochesterbuzz.com/', 'The-Buzz-989.png', 0, 0, 0, 1, '2011-11-11', 12272, 0, '2017-01-01 00:00:00', 0), \
('Legends-967', '', 'http://96.31.83.86:8200/', 'http://www.967wptr.com/', 'Legends-967.png', 0, 0, 0, 1, '2011-11-11', 12273, 0, '2017-01-01 00:00:00', 0), \
('Endless-Testimonies', '', 'http://96.31.83.86:8200/', '', 'Endless-Testimonies.png', 0, 0, 0, 1, '2011-11-11', 12274, 0, '2017-01-01 00:00:00', 0), \
('Blenz-Hit-Music--Talk-Radio', '', 'http://96.31.83.86:8200/', 'http://www.blenz.co.nz/', 'Blenz-Hit-Music--Talk-Radio.png', 0, 0, 0, 1, '2011-11-11', 12275, 0, '2017-01-01 00:00:00', 0), \
('Radio-3-Net-Beatles', '', 'http://96.31.83.86:8200/', 'http://www.radio3net.ro/', 'Radio-3-Net-Beatles.png', 0, 0, 0, 1, '2011-11-11', 12276, 0, '2017-01-01 00:00:00', 0), \
('Radio-Enjoy', '', 'http://96.31.83.86:8200/', 'http://www.radioenjoy.ru/', 'Radio-Enjoy.png', 0, 0, 0, 1, '2011-11-11', 12277, 0, '2017-01-01 00:00:00', 0), \
('Zion-Lutheran-Church', '', 'http://96.31.83.86:8200/', '', 'Zion-Lutheran-Church.png', 0, 0, 0, 1, '2011-11-11', 12278, 0, '2017-01-01 00:00:00', 0), \
('Eddie-Murphy', '', 'http://96.31.83.86:8200/', '', 'Eddie-Murphy.png', 0, 0, 0, 1, '2011-11-11', 12279, 0, '2017-01-01 00:00:00', 0), \
('Bigvibez-Radio', '', 'http://96.31.83.86:8200/', 'http://www.bigvibez.com/', 'Bigvibez-Radio.png', 0, 0, 0, 1, '2011-11-11', 12280, 0, '2017-01-01 00:00:00', 0), \
('Classic-Hits-1057-FM', '', 'http://96.31.83.86:8200/', 'http://redrockonair.com/wxcx/', 'Classic-Hits-1057-FM.png', 0, 0, 0, 1, '2011-11-11', 12281, 0, '2017-01-01 00:00:00', 0), \
('Smooth-Jazz-Through-The-Night', '', 'http://96.31.83.86:8200/', '', 'Smooth-Jazz-Through-The-Night.png', 0, 0, 0, 1, '2011-11-11', 12282, 0, '2017-01-01 00:00:00', 0), \
('The-River-939', '', 'http://96.31.83.86:8200/', 'http://www.939theriverradio.com/', 'The-River-939.png', 0, 0, 0, 1, '2011-11-11', 12283, 0, '2017-01-01 00:00:00', 0), \
('Gary-Louris', '', 'http://96.31.83.86:8200/', '', 'Gary-Louris.png', 0, 0, 0, 1, '2011-11-11', 12284, 0, '2017-01-01 00:00:00', 0), \
('The-Sound-941', '', 'http://96.31.83.86:8200/', 'http://www.941thesound.com/', 'The-Sound-941.png', 0, 0, 0, 1, '2011-11-11', 12285, 0, '2017-01-01 00:00:00', 0), \
('AFI', '', 'http://96.31.83.86:8200/', '', 'AFI.png', 0, 0, 0, 1, '2011-11-11', 12286, 0, '2017-01-01 00:00:00', 0), \
('Real-Country-1050', '', 'http://96.31.83.86:8200/', 'http://www.wwicradio.com/', 'Real-Country-1050.png', 0, 0, 0, 1, '2011-11-11', 12287, 0, '2017-01-01 00:00:00', 0), \
('ESPN-96-1-961', '', 'http://96.31.83.86:8200/', 'http://www.espn961.com', 'ESPN-96-1-961.png', 0, 0, 0, 1, '2011-11-11', 12288, 0, '2017-01-01 00:00:00', 0), \
('Sunny-983', '', 'http://96.31.83.86:8200/', 'http://socialstreamingplayer.crystalmedianetworks.com/radio/kxgt', 'Sunny-983.png', 0, 0, 0, 1, '2011-11-11', 12289, 0, '2017-01-01 00:00:00', 0), \
('Radio-Popolare-1076', 'la storica emittente della sinistra milanese', 'http://96.31.83.86:8200/', 'http://www.radiopopolare.it/', 'Radio-Popolare-1076.png', 0, 0, 0, 1, '2011-11-11', 12290, 0, '2017-01-01 00:00:00', 0), \
('Common-Man-and-The-Torg', '', 'http://96.31.83.86:8200/', '', 'Common-Man-and-The-Torg.png', 0, 0, 0, 1, '2011-11-11', 12291, 0, '2017-01-01 00:00:00', 0), \
('Tan-Talk-1340', '', 'http://96.31.83.86:8200/', 'http://www.tantalk1340.com/', 'Tan-Talk-1340.png', 0, 0, 0, 1, '2011-11-11', 12292, 0, '2017-01-01 00:00:00', 0), \
('TSN-Radio-1290', '', 'http://96.31.83.86:8200/', 'http://www.tsn.ca/winnipeg/', 'TSN-Radio-1290.png', 0, 0, 0, 1, '2011-11-11', 12293, 0, '2017-01-01 00:00:00', 0), \
('Zola-Jesus', '', 'http://96.31.83.86:8200/', '', 'Zola-Jesus.png', 0, 0, 0, 1, '2011-11-11', 12294, 0, '2017-01-01 00:00:00', 0), \
('Chuck-Esposito-Polka-Show', '', 'http://96.31.83.86:8200/', '', 'Chuck-Esposito-Polka-Show.png', 0, 0, 0, 1, '2011-11-11', 12295, 0, '2017-01-01 00:00:00', 0), \
('KKAR-1290', '', 'http://96.31.83.86:8200/', 'http://www.1290kkar.com/', 'KKAR-1290.png', 0, 0, 0, 1, '2011-11-11', 12297, 0, '2017-01-01 00:00:00', 0), \
('Nick-James', '', 'http://96.31.83.86:8200/', '', 'Nick-James.png', 0, 0, 0, 1, '2011-11-11', 12298, 0, '2017-01-01 00:00:00', 0), \
('Mercenary', '', 'http://96.31.83.86:8200/', '', 'Mercenary.png', 0, 0, 0, 1, '2011-11-11', 12299, 0, '2017-01-01 00:00:00', 0), \
('SportsNation', '', 'http://96.31.83.86:8200/', '', 'SportsNation.png', 0, 0, 0, 1, '2011-11-11', 12300, 0, '2017-01-01 00:00:00', 0), \
('Latin-Jazz', '', 'http://96.31.83.86:8200/', '', 'Latin-Jazz.png', 0, 0, 0, 1, '2011-11-11', 12301, 0, '2017-01-01 00:00:00', 0), \
('Lite-1041', 'KLTI-FM (104.1 FM, &quot;Lite 104.1&quot;) is an American radio station broadcasting an adult contemporary music format.', 'http://96.31.83.86:8200/', 'http://www.lite1041.com/', 'Lite-1041.png', 0, 0, 0, 1, '2011-11-11', 12302, 0, '2017-01-01 00:00:00', 0), \
('KXNO-1460', '', 'http://96.31.83.86:8200/', 'http://www.kxno.com/', 'KXNO-1460.png', 0, 0, 0, 1, '2011-11-11', 12303, 0, '2017-01-01 00:00:00', 0), \
('USC-Football-Pre-Game', '', 'http://96.31.83.86:8200/', '', 'USC-Football-Pre-Game.png', 0, 0, 0, 1, '2011-11-11', 12304, 0, '2017-01-01 00:00:00', 0), \
('Nusrat-Fateh-Ali-Khan', '', 'http://96.31.83.86:8200/', '', 'Nusrat-Fateh-Ali-Khan.png', 0, 0, 0, 1, '2011-11-11', 12305, 0, '2017-01-01 00:00:00', 0), \
('Sportsgram-Channel-2', '', 'http://96.31.83.86:8200/', 'http://www.sportsgram.net/', 'Sportsgram-Channel-2.png', 0, 0, 0, 1, '2011-11-11', 12306, 0, '2017-01-01 00:00:00', 0), \
('Strength-For-Living', '', 'http://96.31.83.86:8200/', '', 'Strength-For-Living.png', 0, 0, 0, 1, '2011-11-11', 12307, 0, '2017-01-01 00:00:00', 0), \
('Radio-Valentina-9610', '', 'http://96.31.83.86:8200/', 'http://www.radiovalentina.com/', 'Radio-Valentina-9610.png', 0, 0, 0, 1, '2011-11-11', 12308, 0, '2017-01-01 00:00:00', 0), \
('Radio-Mojo-950', '', 'http://96.31.83.86:8200/', 'http://www.kprcradio.com/', 'Radio-Mojo-950.png', 0, 0, 0, 1, '2011-11-11', 12309, 0, '2017-01-01 00:00:00', 0), \
('Race-Day-Extra', '', 'http://96.31.83.86:8200/', '', 'Race-Day-Extra.png', 0, 0, 0, 1, '2011-11-11', 12310, 0, '2017-01-01 00:00:00', 0), \
('Z-1075', '', 'http://96.31.83.86:8200/', 'http://www.z1075.com/', 'Z-1075.png', 0, 0, 0, 1, '2011-11-11', 12311, 0, '2017-01-01 00:00:00', 0), \
('Radio-Weirdsville', 'All October we&#39;re playing the great Halloween comedy hits like Monster Mash (would you believe SIX versions?), Purple People Eater, Ghostbusters, Will Smith, Elvira, The Blob, Werewolves From Lond', 'http://96.31.83.86:8200/', 'http://www.neverendingwonder.com/weirdsville.htm', 'Radio-Weirdsville.png', 0, 0, 0, 1, '2011-11-11', 12312, 0, '2017-01-01 00:00:00', 0), \
('WJVL-999', '', 'http://96.31.83.86:8200/', 'http://www.wjvl.com/', 'WJVL-999.png', 0, 0, 0, 1, '2011-11-11', 12313, 0, '2017-01-01 00:00:00', 0), \
('WebmasterRadioFM', 'Offering 100% original programming led by leaders in their respective industries, our stellar line-up of radio programming covers everything in online marketing including SEO, affiliate marketing, adv', 'http://96.31.83.86:8200/', 'http://www.webmasterradio.fm/', 'WebmasterRadioFM.png', 0, 0, 0, 1, '2011-11-11', 12314, 0, '2017-01-01 00:00:00', 0), \
('WPWT-870', '', 'http://96.31.83.86:8200/', 'http://www.powertalk870.com/', 'WPWT-870.png', 0, 0, 0, 1, '2011-11-11', 12315, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-710', '', 'http://96.31.83.86:8200/', 'http://espn.go.com/losangeles/radio/', 'ESPN-Radio-710.png', 0, 0, 0, 1, '2011-11-11', 12316, 0, '2017-01-01 00:00:00', 0), \
('Piano-Jazz-on-JAZZRADIOcom', 'Enjoy these historic and contemporary piano jazz masters.', 'http://96.31.83.86:8200/', 'http://www.jazzradio.com/', 'Piano-Jazz-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 12317, 0, '2017-01-01 00:00:00', 0), \
('The-Breeze-937', '', 'http://96.31.83.86:8200/', 'http://www.wgylfm.com/', 'The-Breeze-937.png', 0, 0, 0, 1, '2011-11-11', 12318, 0, '2017-01-01 00:00:00', 0), \
('Three-Days-Grace', '', 'http://96.31.83.86:8200/', '', 'Three-Days-Grace.png', 0, 0, 0, 1, '2011-11-11', 12319, 0, '2017-01-01 00:00:00', 0), \
('Panda-Pop-Radio', 'Pandapopradio.com', 'http://96.31.83.86:8200/', 'http://www.pandapopradio.com/', 'Panda-Pop-Radio.png', 0, 0, 0, 1, '2011-11-11', 12320, 0, '2017-01-01 00:00:00', 0), \
('95-Triple-X-955', '', 'http://96.31.83.86:8200/', 'http://www.95triplex.com/', '95-Triple-X-955.png', 0, 0, 0, 1, '2011-11-11', 12322, 0, '2017-01-01 00:00:00', 0), \
('KISS-981', '', 'http://96.31.83.86:8200/', 'http://www.literockkiss.com/', 'KISS-981.png', 0, 0, 0, 1, '2011-11-11', 12323, 0, '2017-01-01 00:00:00', 0), \
('Open-Mic-Live', '', 'http://96.31.83.86:8200/', '', 'Open-Mic-Live.png', 0, 0, 0, 1, '2011-11-11', 12324, 0, '2017-01-01 00:00:00', 0), \
('KWED-1580', '', 'http://96.31.83.86:8200/', 'http://www.kwed1580.com/', 'KWED-1580.png', 0, 0, 0, 1, '2011-11-11', 12325, 0, '2017-01-01 00:00:00', 0), \
('MMaRchives-933', '', 'http://96.31.83.86:8200/', 'http://www.wmmr.com/music/mmarchives/', 'MMaRchives-933.png', 0, 0, 0, 1, '2011-11-11', 12326, 0, '2017-01-01 00:00:00', 0), \
('The-VILL-971', '', 'http://96.31.83.86:8200/', 'http://www.971thevill.com/', 'The-VILL-971.png', 0, 0, 0, 1, '2011-11-11', 12327, 0, '2017-01-01 00:00:00', 0);";


const char *radio_station_setupsql38="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('KYEE-943', '', 'http://96.31.83.86:8200/', 'http://www.totacc.com/94Key/', 'KYEE-943.png', 0, 0, 0, 1, '2011-11-11', 12328, 0, '2017-01-01 00:00:00', 0), \
('Billy-Ocean', '', 'http://96.31.83.86:8200/', '', 'Billy-Ocean.png', 0, 0, 0, 1, '2011-11-11', 12329, 0, '2017-01-01 00:00:00', 0), \
('Four-Diegos', '', 'http://96.31.83.86:8200/', '', 'Four-Diegos.png', 0, 0, 0, 1, '2011-11-11', 12330, 0, '2017-01-01 00:00:00', 0), \
('WLNZ-897', '89.7 FM WLNZ is the student-operated radio station at Lansing Community College and is Lansing Michigans source for Jazz, Blues, and News.', 'http://96.31.83.86:8200/', 'http://www.lcc.edu/radio/', 'WLNZ-897.png', 0, 0, 0, 1, '2011-11-11', 12331, 0, '2017-01-01 00:00:00', 0), \
('KQWB-1660', '', 'http://96.31.83.86:8200/', 'http://www.espnfargo.com/', 'KQWB-1660.png', 0, 0, 0, 1, '2011-11-11', 12332, 0, '2017-01-01 00:00:00', 0), \
('The-Abominable-Iron-Sloth', '', 'http://96.31.83.86:8200/', '', 'The-Abominable-Iron-Sloth.png', 0, 0, 0, 1, '2011-11-11', 12333, 0, '2017-01-01 00:00:00', 0), \
('The-Doobie-Brothers', '', 'http://96.31.83.86:8200/', '', 'The-Doobie-Brothers.png', 0, 0, 0, 1, '2011-11-11', 12334, 0, '2017-01-01 00:00:00', 0), \
('Hilary-Duff', '', 'http://96.31.83.86:8200/', '', 'Hilary-Duff.png', 0, 0, 0, 1, '2011-11-11', 12335, 0, '2017-01-01 00:00:00', 0), \
('The-House-Of-Love', 'The House Of Love is a web radio streaming 24h at day the best tunes of the history of House Music!', 'http://96.31.83.86:8200/', 'http://www.radionomy.com/en/radio/thehouseoflove-radio', 'The-House-Of-Love.png', 0, 0, 0, 1, '2011-11-11', 12336, 0, '2017-01-01 00:00:00', 0), \
('The-Blitz', '', 'http://96.31.83.86:8200/', '', 'The-Blitz.png', 0, 0, 0, 1, '2011-11-11', 12337, 0, '2017-01-01 00:00:00', 0), \
('KISS-951', '', 'http://96.31.83.86:8200/', 'http://www.mykiss951.com/', 'KISS-951.png', 0, 0, 0, 1, '2011-11-11', 12338, 0, '2017-01-01 00:00:00', 0), \
('KJZZ-915', '', 'http://96.31.83.86:8200/', 'http://www.kjzz.org/', 'KJZZ-915.png', 0, 0, 0, 1, '2011-11-11', 12339, 0, '2017-01-01 00:00:00', 0), \
('New-Order', '', 'http://96.31.83.86:8200/', '', 'New-Order.png', 0, 0, 0, 1, '2011-11-11', 12340, 0, '2017-01-01 00:00:00', 0), \
('The-Official-Chart-Update-with-Greg-James', '', 'http://96.31.83.86:8200/', '', 'The-Official-Chart-Update-with-Greg-James.png', 0, 0, 0, 1, '2011-11-11', 12341, 0, '2017-01-01 00:00:00', 0), \
('Variety-Radio-1065', 'Enjoy the best variety in music from KBVA-FM, Variety 106.5.  We&#39;re locally owned and locally programmed.  Our music spans a great variety of songs and artists.  Enjoy the best variety in music fr', 'http://96.31.83.86:8200/', 'http://variety1065.com/', 'Variety-Radio-1065.png', 0, 0, 0, 1, '2011-11-11', 12342, 0, '2017-01-01 00:00:00', 0), \
('Rejoice-Sunday-Morning', '', 'http://96.31.83.86:8200/', '', 'Rejoice-Sunday-Morning.png', 0, 0, 0, 1, '2011-11-11', 12343, 0, '2017-01-01 00:00:00', 0), \
('St-Louis-Rams-Pre-Game-Show', '', 'http://96.31.83.86:8200/', '', 'St-Louis-Rams-Pre-Game-Show.png', 0, 0, 0, 1, '2011-11-11', 12344, 0, '2017-01-01 00:00:00', 0), \
('Westwood-One-NCAA-Mens-Lacrosse-Championship', '', 'http://96.31.83.86:8200/', '', 'Westwood-One-NCAA-Mens-Lacrosse-Championship.png', 0, 0, 0, 1, '2011-11-11', 12345, 0, '2017-01-01 00:00:00', 0), \
('WMTR-FM-961', 'Best artists, best variety including... Alanis Morisette, Bryan Adams, John Mellencamp, The Goo Goo Dolls, Genesis, The Eagles, Hootie and the Blowfish and much more!!<BR><BR>WMTR Radio is locally own', 'http://96.31.83.86:8200/', 'http://www.961wmtr.com/', 'WMTR-FM-961.png', 0, 0, 0, 1, '2011-11-11', 12346, 0, '2017-01-01 00:00:00', 0), \
('KXCI-913', 'KXCI 91.3FM is Tucson and Southern Arizona&#39;s Community Radio, broadcasting eclectic commercial-free music across all genres since 1983. Do you love great music programmed by great volunteers who a', 'http://96.31.83.86:8200/', 'http://www.kxci.org/', 'KXCI-913.png', 0, 0, 0, 1, '2011-11-11', 12347, 0, '2017-01-01 00:00:00', 0), \
('Thisismynext', '', 'http://96.31.83.86:8200/', '', 'Thisismynext.png', 0, 0, 0, 1, '2011-11-11', 12348, 0, '2017-01-01 00:00:00', 0), \
('Mystic-Sound', '', 'http://96.31.83.86:8200/', '', 'Mystic-Sound.png', 0, 0, 0, 1, '2011-11-11', 12349, 0, '2017-01-01 00:00:00', 0), \
('WQXZ-1039', 'Nothing appeals to Americas largest demographic group, boomers, like Good Times and Great Oldies!', 'http://96.31.83.86:8200/', 'http://www.houstoncountyradio.com/WQXZ.html', 'WQXZ-1039.png', 0, 0, 0, 1, '2011-11-11', 12350, 0, '2017-01-01 00:00:00', 0), \
('Dark-Tranquillity', '', 'http://96.31.83.86:8200/', '', 'Dark-Tranquillity.png', 0, 0, 0, 1, '2011-11-11', 12351, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1290', '', 'http://96.31.83.86:8200/', 'http://www.espn1290.com/', 'ESPN-Radio-1290.png', 0, 0, 0, 1, '2011-11-11', 12352, 0, '2017-01-01 00:00:00', 0), \
('Hollywood-Confidential', '', 'http://96.31.83.86:8200/', '', 'Hollywood-Confidential.png', 0, 0, 0, 1, '2011-11-11', 12353, 0, '2017-01-01 00:00:00', 0), \
('La-Mera-Buena-1075', '107.5 FM La Mera Buena FM Stereo, la estacin preferida de las Ciudades Gemelas, transmitiendo en espaol 24 horas al da los 365 das del ao con lo major de la programacin musical y deportiva', 'http://96.31.83.86:8200/', 'http://www.lamerabuena107fm.com/', 'La-Mera-Buena-1075.png', 0, 0, 0, 1, '2011-11-11', 12354, 0, '2017-01-01 00:00:00', 0), \
('Classical-24-with-John-Zech', '', 'http://96.31.83.86:8200/', '', 'Classical-24-with-John-Zech.png', 0, 0, 0, 1, '2011-11-11', 12355, 0, '2017-01-01 00:00:00', 0), \
('Christina-Milian', '', 'http://96.31.83.86:8200/', '', 'Christina-Milian.png', 0, 0, 0, 1, '2011-11-11', 12356, 0, '2017-01-01 00:00:00', 0), \
('WMZQ-FM-987', '', 'http://96.31.83.86:8200/', 'http://www.wmzq.com/', 'WMZQ-FM-987.png', 0, 0, 0, 1, '2011-11-11', 12357, 0, '2017-01-01 00:00:00', 0), \
('Spoon', '', 'http://96.31.83.86:8200/', '', 'Spoon.png', 0, 0, 0, 1, '2011-11-11', 12358, 0, '2017-01-01 00:00:00', 0), \
('Montgomery-Gentry', '', 'http://96.31.83.86:8200/', '', 'Montgomery-Gentry.png', 0, 0, 0, 1, '2011-11-11', 12359, 0, '2017-01-01 00:00:00', 0), \
('St-Louis-Public-Radio-907', 'A trusted source of information and entertainment that opens minds and nourishes the spirit.', 'http://96.31.83.86:8200/', 'http://www.stlpublicradio.org/', 'St-Louis-Public-Radio-907.png', 0, 0, 0, 1, '2011-11-11', 12360, 0, '2017-01-01 00:00:00', 0), \
('Freestyle-Dance', '', 'http://96.31.83.86:8200/', 'http://www.freestyle-dance.com/', 'Freestyle-Dance.png', 0, 0, 0, 1, '2011-11-11', 12361, 0, '2017-01-01 00:00:00', 0), \
('Svartahrid', '', 'http://96.31.83.86:8200/', '', 'Svartahrid.png', 0, 0, 0, 1, '2011-11-11', 12362, 0, '2017-01-01 00:00:00', 0), \
('River-927', '', 'http://96.31.83.86:8200/', 'http://www.dakotaradiogroup.com/', 'River-927.png', 0, 0, 0, 1, '2011-11-11', 12363, 0, '2017-01-01 00:00:00', 0), \
('WZAK-931', '', 'http://96.31.83.86:8200/', 'http://www.wzakcleveland.com/', 'WZAK-931.png', 0, 0, 0, 1, '2011-11-11', 12364, 0, '2017-01-01 00:00:00', 0), \
('Chicago-Public-Radio-915', '', 'http://96.31.83.86:8200/', 'http://www.chicagopublicradio.org/', 'Chicago-Public-Radio-915.png', 0, 0, 0, 1, '2011-11-11', 12365, 0, '2017-01-01 00:00:00', 0), \
('1037-The-Point', '', 'http://96.31.83.86:8200/', 'http://www.1037thepoint.net/', '1037-The-Point.png', 0, 0, 0, 1, '2011-11-11', 12366, 0, '2017-01-01 00:00:00', 0), \
('The-Mancow-Show', '', 'http://96.31.83.86:8200/', '', 'The-Mancow-Show.png', 0, 0, 0, 1, '2011-11-11', 12367, 0, '2017-01-01 00:00:00', 0), \
('Q-107-1073', '', 'http://96.31.83.86:8200/', 'http://www.q107radio.com/', 'Q-107-1073.png', 0, 0, 0, 1, '2011-11-11', 12368, 0, '2017-01-01 00:00:00', 0), \
('WWBL-1065', '', 'http://96.31.83.86:8200/', 'http://www.wwbl.com/', 'WWBL-1065.png', 0, 0, 0, 1, '2011-11-11', 12369, 0, '2017-01-01 00:00:00', 0), \
('Kay-Starr', '', 'http://96.31.83.86:8200/', '', 'Kay-Starr.png', 0, 0, 0, 1, '2011-11-11', 12370, 0, '2017-01-01 00:00:00', 0), \
('Radio-Healthy', '', 'http://96.31.83.86:8200/', 'http://www.radiohealthy.com/', 'Radio-Healthy.png', 0, 0, 0, 1, '2011-11-11', 12371, 0, '2017-01-01 00:00:00', 0), \
('Rick-Edelman', '', 'http://96.31.83.86:8200/', '', 'Rick-Edelman.png', 0, 0, 0, 1, '2011-11-11', 12372, 0, '2017-01-01 00:00:00', 0), \
('Cherry-Poppin-Daddies', '', 'http://96.31.83.86:8200/', '', 'Cherry-Poppin-Daddies.png', 0, 0, 0, 1, '2011-11-11', 12373, 0, '2017-01-01 00:00:00', 0), \
('Fantasy-Focus', '', 'http://96.31.83.86:8200/', '', 'Fantasy-Focus.png', 0, 0, 0, 1, '2011-11-11', 12374, 0, '2017-01-01 00:00:00', 0), \
('KIUP-930', '', 'http://96.31.83.86:8200/', 'http://www.radiodurango.com/kiup.asp', 'KIUP-930.png', 0, 0, 0, 1, '2011-11-11', 12375, 0, '2017-01-01 00:00:00', 0), \
('Rugby-WC-Ireland-vs-Wales-Oct-8-2011', '', 'http://96.31.83.86:8200/', '', 'Rugby-WC-Ireland-vs-Wales-Oct-8-2011.png', 0, 0, 0, 1, '2011-11-11', 12376, 0, '2017-01-01 00:00:00', 0), \
('KRJC-953', '', 'http://96.31.83.86:8200/', 'http://www.krjc.com/', 'KRJC-953.png', 0, 0, 0, 1, '2011-11-11', 12377, 0, '2017-01-01 00:00:00', 0), \
('Donald-Fagen', '', 'http://96.31.83.86:8200/', '', 'Donald-Fagen.png', 0, 0, 0, 1, '2011-11-11', 12378, 0, '2017-01-01 00:00:00', 0), \
('Bob-1069', 'Savannah and Hilton Head&#39;s new number one for country music!', 'http://96.31.83.86:8200/', 'http://www.bob1069.com/', 'Bob-1069.png', 0, 0, 0, 1, '2011-11-11', 12379, 0, '2017-01-01 00:00:00', 0), \
('R¡dio-Trans-Mundial-1540', '', 'http://96.31.83.86:8200/', 'http://www.transmundial.com.br/index.php3', 'Radio-Trans-Mundial-1540.png', 0, 0, 0, 1, '2011-11-11', 12380, 0, '2017-01-01 00:00:00', 0), \
('Truckfest-FM', '', 'http://96.31.83.86:8200/', 'http://www.truckfestfm.com', 'Truckfest-FM.png', 0, 0, 0, 1, '2011-11-11', 12381, 0, '2017-01-01 00:00:00', 0), \
('Radio-Ici--Maintenant-952', '', 'http://96.31.83.86:8200/', 'http://www.icietmaintenant.com', 'Radio-Ici--Maintenant-952.png', 0, 0, 0, 1, '2011-11-11', 12382, 0, '2017-01-01 00:00:00', 0), \
('Duran-Duran', '', 'http://96.31.83.86:8200/', '', 'Duran-Duran.png', 0, 0, 0, 1, '2011-11-11', 12383, 0, '2017-01-01 00:00:00', 0), \
('Rock-107-1071', '', 'http://96.31.83.86:8200/', 'http://www.wirx.com/', 'Rock-107-1071.png', 0, 0, 0, 1, '2011-11-11', 12384, 0, '2017-01-01 00:00:00', 0), \
('KAHI-950', '', 'http://96.31.83.86:8200/', 'http://www.kahi.com/', 'KAHI-950.png', 0, 0, 0, 1, '2011-11-11', 12385, 0, '2017-01-01 00:00:00', 0), \
('WCJU-1450', '', 'http://96.31.83.86:8200/', '', 'WCJU-1450.png', 0, 0, 0, 1, '2011-11-11', 12386, 0, '2017-01-01 00:00:00', 0), \
('Joss-Stone', '', 'http://96.31.83.86:8200/', '', 'Joss-Stone.png', 0, 0, 0, 1, '2011-11-11', 12387, 0, '2017-01-01 00:00:00', 0), \
('The-Band-Perry', '', 'http://96.31.83.86:8200/', '', 'The-Band-Perry.png', 0, 0, 0, 1, '2011-11-11', 12388, 0, '2017-01-01 00:00:00', 0), \
('Rock-92-923', 'Rock 92, has been rockin&#39; Carolina for over 25 years. Home of The 2 Guys Named Chris Show.', 'http://96.31.83.86:8200/', 'http://www.rock92.com/', 'Rock-92-923.png', 0, 0, 0, 1, '2011-11-11', 12389, 0, '2017-01-01 00:00:00', 0), \
('Xs-and-Os-with-the-Pros', '', 'http://96.31.83.86:8200/', '', 'Xs-and-Os-with-the-Pros.png', 0, 0, 0, 1, '2011-11-11', 12390, 0, '2017-01-01 00:00:00', 0), \
('Klassik-Radio-922', '', 'http://96.31.83.86:8200/', 'http://www.klassikradio.de/', 'Klassik-Radio-922.png', 0, 0, 0, 1, '2011-11-11', 12391, 0, '2017-01-01 00:00:00', 0), \
('Bonnie-Tyler', '', 'http://96.31.83.86:8200/', '', 'Bonnie-Tyler.png', 0, 0, 0, 1, '2011-11-11', 12393, 0, '2017-01-01 00:00:00', 0), \
('Jason-Mraz', '', 'http://96.31.83.86:8200/', '', 'Jason-Mraz.png', 0, 0, 0, 1, '2011-11-11', 12394, 0, '2017-01-01 00:00:00', 0), \
('Off-The-Field', '', 'http://96.31.83.86:8200/', '', 'Off-The-Field.png', 0, 0, 0, 1, '2011-11-11', 12395, 0, '2017-01-01 00:00:00', 0), \
('Beyond-The-Beat-Generation', '', 'http://96.31.83.86:8200/', 'http://www.beyondthebeatgeneration.com/', 'Beyond-The-Beat-Generation.png', 0, 0, 1, 1, '2011-11-11', 12396, 0, '2011-10-14 16:03:40', 0), \
('WJSU-FM-885', '', 'http://96.31.83.86:8200/', 'http://www.wjsu.org/', 'WJSU-FM-885.png', 0, 0, 0, 1, '2011-11-11', 12397, 0, '2017-01-01 00:00:00', 0), \
('ESPN-850', '', 'http://96.31.83.86:8200/', 'http://www.espncleveland.com/', 'ESPN-850.png', 0, 0, 0, 1, '2011-11-11', 12398, 0, '2017-01-01 00:00:00', 0), \
('The-GFQ-Network', 'Guys From Queens Network Inc.  Is a web based television network covering a wide variety of topics from Technology to Entertainment.  Produced out of our Queens NYC studio, our web shows feature a wid', 'http://96.31.83.86:8200/', 'http://www.guysfromqueens.com/', 'The-GFQ-Network.png', 0, 0, 0, 1, '2011-11-11', 12399, 0, '2017-01-01 00:00:00', 0), \
('Magic-899', '', 'http://96.31.83.86:8200/', 'http://www.magic899.fm/', 'Magic-899.png', 0, 0, 0, 1, '2011-11-11', 12400, 0, '2017-01-01 00:00:00', 0), \
('Next-FM-991', '', 'http://96.31.83.86:8200/', 'http://www.next99fm.com/', 'Next-FM-991.png', 0, 0, 0, 1, '2011-11-11', 12401, 0, '2017-01-01 00:00:00', 0), \
('KTFM-941', '', 'http://96.31.83.86:8200/', 'http://www.ktfm.com/', 'KTFM-941.png', 0, 0, 0, 1, '2011-11-11', 12402, 0, '2017-01-01 00:00:00', 0), \
('Reach-OnAir', 'Don&#39;t just listen to Internet Radio - Be a DJ! Ever wanted to be the person behind the microphone, playing the music you want? Reach OnAir gives you that chance for free! Sign up and book a show o', 'http://96.31.83.86:8200/', 'http://www.reachonair.com/', 'Reach-OnAir.png', 0, 0, 0, 1, '2011-11-11', 12403, 0, '2017-01-01 00:00:00', 0), \
('5-Live-Sport', '', 'http://96.31.83.86:8200/', '', '5-Live-Sport.png', 0, 0, 0, 1, '2011-11-11', 12404, 0, '2017-01-01 00:00:00', 0), \
('Top-5--5', '', 'http://96.31.83.86:8200/', '', 'Top-5--5.png', 0, 0, 0, 1, '2011-11-11', 12405, 0, '2017-01-01 00:00:00', 0), \
('The-Zone-790', '', 'http://96.31.83.86:8200/', 'http://www.790thezone.com/', 'The-Zone-790.png', 0, 0, 0, 1, '2011-11-11', 12406, 0, '2017-01-01 00:00:00', 0), \
('Huey-Morgan', '', 'http://96.31.83.86:8200/', '', 'Huey-Morgan.png', 0, 0, 0, 1, '2011-11-11', 12407, 0, '2017-01-01 00:00:00', 0), \
('Cut-Off-Your-Hands', '', 'http://96.31.83.86:8200/', '', 'Cut-Off-Your-Hands.png', 0, 0, 0, 1, '2011-11-11', 12408, 0, '2017-01-01 00:00:00', 0), \
('KICX-1059', '', 'http://96.31.83.86:8200/', 'http://www.kicx106.com/', 'KICX-1059.png', 0, 0, 0, 1, '2011-11-11', 12409, 0, '2017-01-01 00:00:00', 0), \
('Universitetsradioen-955', '', 'http://96.31.83.86:8200/', 'http://www.uradio.ku.dk/', 'Universitetsradioen-955.png', 0, 0, 0, 1, '2011-11-11', 12410, 0, '2017-01-01 00:00:00', 0), \
('C-101-1013', '', 'http://96.31.83.86:8200/', 'http://www.c101.com/', 'C-101-1013.png', 0, 0, 0, 1, '2011-11-11', 12411, 0, '2017-01-01 00:00:00', 0), \
('FD-Lounge-Radio', '', 'http://96.31.83.86:8200/', 'http://www.fd-lounge-radio.com/', 'FD-Lounge-Radio.png', 0, 0, 0, 1, '2011-11-11', 12412, 0, '2017-01-01 00:00:00', 0), \
('Os-Mutantes', '', 'http://96.31.83.86:8200/', '', 'Os-Mutantes.png', 0, 0, 0, 1, '2011-11-11', 12413, 0, '2017-01-01 00:00:00', 0), \
('Good-Morning-with-Heather-Bambrick', '', 'http://96.31.83.86:8200/', '', 'Good-Morning-with-Heather-Bambrick.png', 0, 0, 0, 1, '2011-11-11', 12414, 0, '2017-01-01 00:00:00', 0), \
('Golden-Sixties-Seventies-Eighties-Radio', 'Un max de musique anglo saxonnes des ann&#233;es 60 &#224; 90! Pop Rock Disco Reggae', 'http://96.31.83.86:8200/', 'http://www.radionomy.com/en/radio/golden-sixties-seventies-eighties-radio', 'Golden-Sixties-Seventies-Eighties-Radio.png', 0, 0, 0, 1, '2011-11-11', 12415, 0, '2017-01-01 00:00:00', 0), \
('1015-LITE-FM', '', 'http://96.31.83.86:8200/', 'http://www.litemiami.com/', '1015-LITE-FM.png', 0, 0, 0, 1, '2011-11-11', 12416, 0, '2017-01-01 00:00:00', 0), \
('Mix-941', '', 'http://96.31.83.86:8200/', 'http://www.mix941fm.com/', 'Mix-941.png', 0, 0, 0, 1, '2011-11-11', 12418, 0, '2017-01-01 00:00:00', 0), \
('Heart-Gloucestershire-1024', 'Heart 102.4 with Warren Moore at breakfast covers Gloucester, Cheltenham, Tewksbury, Stroud, The Forest of Dean and East Cotswolds', 'http://96.31.83.86:8200/', 'http://www.heart.co.uk/gloucestershire/', 'Heart-Gloucestershire-1024.png', 0, 0, 0, 1, '2011-11-11', 12419, 0, '2017-01-01 00:00:00', 0), \
('Radioseven', 'We are the swedish dance floor, and bring you the best of dance, trance and house 24/7. Listen via our mobile app, your computer or your DAB+ radio!', 'http://96.31.83.86:8200/', 'http://www.radioseven.se/', 'Radioseven.png', 0, 0, 0, 1, '2011-11-11', 12420, 0, '2017-01-01 00:00:00', 0), \
('WDJA-1420', 'News Talk format. CNN Radio News LIVE at the top of every hour. Market Watch at the bottom of every hour during the active hours of stock market during the week.', 'http://96.31.83.86:8200/', 'http://www.jammin1420.com/', 'WDJA-1420.png', 0, 0, 0, 1, '2011-11-11', 12421, 0, '2017-01-01 00:00:00', 0), \
('Mecano', '', 'http://96.31.83.86:8200/', '', 'Mecano.png', 0, 0, 0, 1, '2011-11-11', 12422, 0, '2017-01-01 00:00:00', 0), \
('Pontoppidan-Church-Service', '', 'http://96.31.83.86:8200/', '', 'Pontoppidan-Church-Service.png', 0, 0, 0, 1, '2011-11-11', 12423, 0, '2017-01-01 00:00:00', 0), \
('987fm', '', 'http://96.31.83.86:8200/', 'http://www.987fm.com/', '987fm.png', 0, 0, 0, 1, '2011-11-11', 12424, 0, '2017-01-01 00:00:00', 0), \
('Grover-Washington-Jr', '', 'http://96.31.83.86:8200/', '', 'Grover-Washington-Jr.png', 0, 0, 0, 1, '2011-11-11', 12425, 0, '2017-01-01 00:00:00', 0), \
('The-Mike-Nowak-Show', '', 'http://96.31.83.86:8200/', '', 'The-Mike-Nowak-Show.png', 0, 0, 0, 1, '2011-11-11', 12426, 0, '2017-01-01 00:00:00', 0), \
('Marys-Helpers-News-Hour', '', 'http://96.31.83.86:8200/', '', 'Marys-Helpers-News-Hour.png', 0, 0, 0, 1, '2011-11-11', 12427, 0, '2017-01-01 00:00:00', 0), \
('WEEX-1230', '', 'http://96.31.83.86:8200/', 'http://www.espnlv.com/', 'WEEX-1230.png', 0, 0, 0, 1, '2011-11-11', 12428, 0, '2017-01-01 00:00:00', 0), \
('SomaFM-Boot-Liquor', 'Music that shows the ongoing hopelessness in your otherwise dreary life. Americana roots music (or what they used to call Country Western) that&#39;s both musically and lyrically unique. Only from Som', 'http://96.31.83.86:8200/', 'http://somafm.com/', 'SomaFM-Boot-Liquor.png', 0, 0, 0, 1, '2011-11-11', 12429, 0, '2017-01-01 00:00:00', 0), \
('WIMS-1420', '', 'http://96.31.83.86:8200/', 'http://www.wimsradio.com/', 'WIMS-1420.png', 0, 0, 0, 1, '2011-11-11', 12430, 0, '2017-01-01 00:00:00', 0), \
('Mads--Monopolet', '', 'http://96.31.83.86:8200/', '', 'Mads--Monopolet.png', 0, 0, 0, 1, '2011-11-11', 12431, 0, '2017-01-01 00:00:00', 0), \
('Real-Country-1390', '', 'http://96.31.83.86:8200/', 'http://site.realcountry1390.com/', 'Real-Country-1390.png', 0, 0, 0, 1, '2011-11-11', 12432, 0, '2017-01-01 00:00:00', 0), \
('Oregon-Ducks-Pre-Game-Show', '', 'http://96.31.83.86:8200/', '', 'Oregon-Ducks-Pre-Game-Show.png', 0, 0, 0, 1, '2011-11-11', 12433, 0, '2017-01-01 00:00:00', 0), \
('KFKA-1310', '', 'http://96.31.83.86:8200/', 'http://www.1310kfka.com/', 'KFKA-1310.png', 0, 0, 0, 1, '2011-11-11', 12434, 0, '2017-01-01 00:00:00', 0), \
('Just-90s', 'Die besten Hits der Jahre 1990 bis 1999! Eurodisco, Rock, Boygroups, Girlgroups, Techno, House, etc.', 'http://96.31.83.86:8200/', 'http://www.laut.fm/just90s', 'Just-90s.png', 0, 0, 0, 1, '2011-11-11', 12435, 0, '2017-01-01 00:00:00', 0), \
('Mumford--Sons', '', 'http://96.31.83.86:8200/', '', 'Mumford--Sons.png', 0, 0, 0, 1, '2011-11-11', 12436, 0, '2017-01-01 00:00:00', 0), \
('PRI-Public-Radio-International', 'PRI is the leading broadcast provider of global news and cultural perspectives in the US, reaching over 13 million people on almost 900 public radio stations nationwide, on-line at pri.org, and on-dem', 'http://96.31.83.86:8200/', 'http://www.pri.org/', 'PRI-Public-Radio-International.png', 0, 0, 0, 1, '2011-11-11', 12437, 0, '2017-01-01 00:00:00', 0), \
('Y-FM-992', 'The real &quot;makoya&quot; for youth entertainment and edutainment.', 'http://96.31.83.86:8200/', 'http://www.yworld.co.za/', 'Y-FM-992.png', 0, 0, 0, 1, '2011-11-11', 12438, 0, '2017-01-01 00:00:00', 0), \
('Sunny-931', '', 'http://96.31.83.86:8200/', '', 'Sunny-931.png', 0, 0, 0, 1, '2011-11-11', 12439, 0, '2017-01-01 00:00:00', 0), \
('Social-Distortion', '', 'http://96.31.83.86:8200/', '', 'Social-Distortion.png', 0, 0, 0, 1, '2011-11-11', 12440, 0, '2017-01-01 00:00:00', 0), \
('Bob-Welch', '', 'http://96.31.83.86:8200/', '', 'Bob-Welch.png', 0, 0, 0, 1, '2011-11-11', 12441, 0, '2017-01-01 00:00:00', 0), \
('Onslaught-Radio', 'Playing nothing but metal 24 hours a day with unsigned bands and interviews', 'http://96.31.83.86:8200/', 'http://www.onslaughtradio.com/', 'Onslaught-Radio.png', 0, 0, 0, 1, '2011-11-11', 12442, 0, '2017-01-01 00:00:00', 0), \
('Harvey-Lee', '', 'http://96.31.83.86:8200/', '', 'Harvey-Lee.png', 0, 0, 0, 1, '2011-11-11', 12443, 0, '2017-01-01 00:00:00', 0), \
('Morning-Breeze', '', 'http://96.31.83.86:8200/', '', 'Morning-Breeze.png', 0, 0, 0, 1, '2011-11-11', 12444, 0, '2017-01-01 00:00:00', 0), \
('All-Things-Medical', '', 'http://96.31.83.86:8200/', '', 'All-Things-Medical.png', 0, 0, 1, 1, '2011-11-11', 12445, 0, '2011-10-15 20:12:01', 0), \
('WHTC-1450', '', 'http://96.31.83.86:8200/', 'http://www.whtc.com/', 'WHTC-1450.png', 0, 0, 0, 1, '2011-11-11', 12446, 0, '2017-01-01 00:00:00', 0), \
('Janis-Joplin', '', 'http://96.31.83.86:8200/', '', 'Janis-Joplin.png', 0, 0, 0, 1, '2011-11-11', 12447, 0, '2017-01-01 00:00:00', 0), \
('Katy-Parker', '', 'http://96.31.83.86:8200/', '', 'Katy-Parker.png', 0, 0, 0, 1, '2011-11-11', 12448, 0, '2017-01-01 00:00:00', 0), \
('Tori-James', '', 'http://96.31.83.86:8200/', '', 'Tori-James.png', 0, 0, 0, 1, '2011-11-11', 12449, 0, '2017-01-01 00:00:00', 0), \
('Victor-Wooten', '', 'http://96.31.83.86:8200/', '', 'Victor-Wooten.png', 0, 0, 0, 1, '2011-11-11', 12450, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1390', '', 'http://96.31.83.86:8200/', 'http://1390kjox.com/', 'ESPN-Radio-1390.png', 0, 0, 0, 1, '2011-11-11', 12451, 0, '2017-01-01 00:00:00', 0), \
('The-FOX-931', '', 'http://96.31.83.86:8200/', 'http://www.foxrocks.com/', 'The-FOX-931.png', 0, 0, 0, 1, '2011-11-11', 12452, 0, '2017-01-01 00:00:00', 0), \
('Q-105-1055', '', 'http://96.31.83.86:8200/', 'http://www.q105.fm/', 'Q-105-1055.png', 0, 0, 0, 1, '2011-11-11', 12453, 0, '2017-01-01 00:00:00', 0), \
('KHAR-590', '', 'http://96.31.83.86:8200/', 'http://www.khar590.com/', 'KHAR-590.png', 0, 0, 0, 1, '2011-11-11', 12454, 0, '2017-01-01 00:00:00', 0), \
('Fall-Out-Boy', '', 'http://96.31.83.86:8200/', '', 'Fall-Out-Boy.png', 0, 0, 0, 1, '2011-11-11', 12455, 0, '2017-01-01 00:00:00', 0), \
('Q-music-1004', 'Radiostation Q-music is overal te ontvangen op de 100.7 FM, via de kabel of internet. De radiostudio van Q-music is gevestigd in Amsterdam.', 'http://96.31.83.86:8200/', 'http://www.q-music.nl/', 'Q-music-1004.png', 0, 0, 0, 1, '2011-11-11', 12456, 0, '2017-01-01 00:00:00', 0), \
('KLAC-570', '', 'http://96.31.83.86:8200/', 'http://www.570klac.com', 'KLAC-570.png', 0, 0, 0, 1, '2011-11-11', 12457, 0, '2017-01-01 00:00:00', 0), \
('Sammy-Hagar', '', 'http://96.31.83.86:8200/', '', 'Sammy-Hagar.png', 0, 0, 0, 1, '2011-11-11', 12458, 0, '2017-01-01 00:00:00', 0), \
('Radio-Randers-1049', '', 'http://96.31.83.86:8200/', 'http://www.radioranders.dk/', 'Radio-Randers-1049.png', 0, 0, 0, 1, '2011-11-11', 12459, 0, '2017-01-01 00:00:00', 0), \
('Wynton-Marsalis-Quartet', '', 'http://96.31.83.86:8200/', '', 'Wynton-Marsalis-Quartet.png', 0, 0, 0, 1, '2011-11-11', 12460, 0, '2017-01-01 00:00:00', 0), \
('WBAL-1090', '', 'http://96.31.83.86:8200/', 'http://www.wbal.com/', 'WBAL-1090.png', 0, 0, 0, 1, '2011-11-11', 12461, 0, '2017-01-01 00:00:00', 0), \
('981-The-Hawk', '', 'http://96.31.83.86:8200/', 'http://www.981thehawk.com/', '981-The-Hawk.png', 0, 0, 0, 1, '2011-11-11', 12462, 0, '2017-01-01 00:00:00', 0), \
('Born-Again-Broadcast', '', 'http://96.31.83.86:8200/', '', 'Born-Again-Broadcast.png', 0, 0, 0, 1, '2011-11-11', 12463, 0, '2017-01-01 00:00:00', 0), \
('Radio-Humleborg-1043', '', 'http://96.31.83.86:8200/', 'http://humleborg.dk/', 'Radio-Humleborg-1043.png', 0, 0, 0, 1, '2011-11-11', 12464, 0, '2017-01-01 00:00:00', 0), \
('Zoot-Sims', '', 'http://96.31.83.86:8200/', '', 'Zoot-Sims.png', 0, 0, 0, 1, '2011-11-11', 12465, 0, '2017-01-01 00:00:00', 0), \
('Whats-Wright', '', 'http://96.31.83.86:8200/', '', 'Whats-Wright.png', 0, 0, 0, 1, '2011-11-11', 12466, 0, '2017-01-01 00:00:00', 0), \
('Doves', '', 'http://96.31.83.86:8200/', '', 'Doves.png', 0, 0, 0, 1, '2011-11-11', 12467, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-Coming-Down', '', 'http://96.31.83.86:8200/', '', 'Sunday-Morning-Coming-Down.png', 0, 0, 0, 1, '2011-11-11', 12468, 0, '2017-01-01 00:00:00', 0), \
('Read-the-Bible-together', '', 'http://96.31.83.86:8200/', '', 'Read-the-Bible-together.png', 0, 0, 0, 1, '2011-11-11', 12469, 0, '2017-01-01 00:00:00', 0), \
('The-River-949', 'The fact that 94.9 the River is challenging to describe in a sentence (or a page, for that matter) is exactly what makes it appealing to an adult audience who prefers genuine diversity in music - and ', 'http://96.31.83.86:8200/', 'http://www.riverinteractive.com/', 'The-River-949.png', 0, 0, 0, 1, '2011-11-11', 12470, 0, '2017-01-01 00:00:00', 0), \
('Victory-Radio-1009', '', 'http://96.31.83.86:8200/', 'http://www.kaknradio.org/', 'Victory-Radio-1009.png', 0, 0, 0, 1, '2011-11-11', 12471, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1390', '', 'http://96.31.83.86:8200/', 'http://www.espn1390am.com/', 'ESPN-1390.png', 0, 0, 0, 1, '2011-11-11', 12472, 0, '2017-01-01 00:00:00', 0), \
('The-Max-1023', '', 'http://96.31.83.86:8200/', 'http://www.themaxfm.com/', 'The-Max-1023.png', 0, 0, 0, 1, '2011-11-11', 12473, 0, '2017-01-01 00:00:00', 0), \
('Cymbals-Eat-Guitars', '', 'http://96.31.83.86:8200/', '', 'Cymbals-Eat-Guitars.png', 0, 0, 0, 1, '2011-11-11', 12474, 0, '2017-01-01 00:00:00', 0), \
('Eagle-969', '', 'http://96.31.83.86:8200/', 'http://www.eagle969.com/', 'Eagle-969.png', 0, 0, 0, 1, '2011-11-11', 12475, 0, '2017-01-01 00:00:00', 0), \
('The-Answer', '', 'http://96.31.83.86:8200/', '', 'The-Answer.png', 0, 0, 0, 1, '2011-11-11', 12476, 0, '2017-01-01 00:00:00', 0), \
('Russell-Malone', '', 'http://96.31.83.86:8200/', '', 'Russell-Malone.png', 0, 0, 0, 1, '2011-11-11', 12477, 0, '2017-01-01 00:00:00', 0), \
('WBBM-Newsradio-780', 'WBBM Newsradio 780 has been the consistent all news radio voice in Chicagoland for 40 years and is now streaming online for free. Listen online to Chicago news as it happens around the clock, along wi', 'http://96.31.83.86:8200/', 'http://www.wbbm780.com/', 'WBBM-Newsradio-780.png', 0, 0, 0, 1, '2011-11-11', 12478, 0, '2017-01-01 00:00:00', 0), \
('Tina-Arena', '', 'http://96.31.83.86:8200/', '', 'Tina-Arena.png', 0, 0, 0, 1, '2011-11-11', 12479, 0, '2017-01-01 00:00:00', 0), \
('Scott-Mills', '', 'http://96.31.83.86:8200/', '', 'Scott-Mills.png', 0, 0, 0, 1, '2011-11-11', 12480, 0, '2017-01-01 00:00:00', 0), \
('Eric-Church', '', 'http://96.31.83.86:8200/', '', 'Eric-Church.png', 0, 0, 0, 1, '2011-11-11', 12481, 0, '2017-01-01 00:00:00', 0), \
('Kellers-Cellar', '', 'http://96.31.83.86:8200/', '', 'Kellers-Cellar.png', 0, 0, 0, 1, '2011-11-11', 12482, 0, '2017-01-01 00:00:00', 0), \
('Shawn-Mullins', '', 'http://96.31.83.86:8200/', '', 'Shawn-Mullins.png', 0, 0, 0, 1, '2011-11-11', 12483, 0, '2017-01-01 00:00:00', 0), \
('North-Carolina-Tar-Heels-at-Clemson-Tigers-Oct-22-2011', '', 'http://96.31.83.86:8200/', '', 'North-Carolina-Tar-Heels-at-Clemson-Tigers-Oct-22-2011.png', 0, 0, 0, 1, '2011-11-11', 12484, 0, '2017-01-01 00:00:00', 0), \
('Talking-Baseball-with-Dutch', '', 'http://96.31.83.86:8200/', '', 'Talking-Baseball-with-Dutch.png', 0, 0, 0, 1, '2011-11-11', 12485, 0, '2017-01-01 00:00:00', 0), \
('929-Country', '', 'http://96.31.83.86:8200/', 'http://www.waac929.com/', '929-Country.png', 0, 0, 0, 1, '2011-11-11', 12486, 0, '2017-01-01 00:00:00', 0), \
('Reagan-Youth', '', 'http://96.31.83.86:8200/', '', 'Reagan-Youth.png', 0, 0, 0, 1, '2011-11-11', 12487, 0, '2017-01-01 00:00:00', 0), \
('Donnie-Iris', '', 'http://96.31.83.86:8200/', '', 'Donnie-Iris.png', 0, 0, 0, 1, '2011-11-11', 12488, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1230', '', 'http://96.31.83.86:8200/', 'http://www.espnradio1230.com/', 'ESPN-Radio-1230.png', 0, 0, 0, 1, '2011-11-11', 12489, 0, '2017-01-01 00:00:00', 0), \
('The-Brand-New-Heavies', '', 'http://96.31.83.86:8200/', '', 'The-Brand-New-Heavies.png', 0, 0, 0, 1, '2011-11-11', 12490, 0, '2017-01-01 00:00:00', 0), \
('Suicide-Silence', '', 'http://96.31.83.86:8200/', '', 'Suicide-Silence.png', 0, 0, 0, 1, '2011-11-11', 12491, 0, '2017-01-01 00:00:00', 0), \
('Jaguar-Skillz', '', 'http://96.31.83.86:8200/', '', 'Jaguar-Skillz.png', 0, 0, 0, 1, '2011-11-11', 12492, 0, '2017-01-01 00:00:00', 0), \
('WILM-NewsRadio-1450', '', 'http://96.31.83.86:8200/', 'http://www.wilm.com/', 'WILM-NewsRadio-1450.png', 0, 0, 0, 1, '2011-11-11', 12493, 0, '2017-01-01 00:00:00', 0), \
('WKDU-917', '', 'http://96.31.83.86:8200/', 'http://www.wkdu.org/', 'WKDU-917.png', 0, 0, 0, 1, '2011-11-11', 12494, 0, '2017-01-01 00:00:00', 0), \
('The-Acoustic-Storm', '', 'http://96.31.83.86:8200/', '', 'The-Acoustic-Storm.png', 0, 0, 0, 1, '2011-11-11', 12495, 0, '2017-01-01 00:00:00', 0), \
('IRIE-ATL-Radio-1430', '', 'http://96.31.83.86:8200/', 'http://www.irieatl.com/', 'IRIE-ATL-Radio-1430.png', 0, 0, 0, 1, '2011-11-11', 12496, 0, '2017-01-01 00:00:00', 0), \
('KWVA-881', '', 'http://96.31.83.86:8200/', 'http://gladstone.uoregon.edu/~kwva/', 'KWVA-881.png', 0, 0, 0, 1, '2011-11-11', 12497, 0, '2017-01-01 00:00:00', 0), \
('The-Sunburst-Band', '', 'http://96.31.83.86:8200/', '', 'The-Sunburst-Band.png', 0, 0, 0, 1, '2011-11-11', 12498, 0, '2017-01-01 00:00:00', 0), \
('Silverstein', '', 'http://96.31.83.86:8200/', '', 'Silverstein.png', 0, 0, 0, 1, '2011-11-11', 12499, 0, '2017-01-01 00:00:00', 0), \
('Con-Ciencia-Digital', '', 'http://96.31.83.86:8200/', '', 'Con-Ciencia-Digital.png', 0, 0, 0, 1, '2011-11-11', 12500, 0, '2017-01-01 00:00:00', 0), \
('KLCN-910', '', 'http://96.31.83.86:8200/', '', 'KLCN-910.png', 0, 0, 0, 1, '2011-11-11', 12501, 0, '2017-01-01 00:00:00', 0), \
('Z-923', '', 'http://96.31.83.86:8200/', 'http://www.kiiz.com/', 'Z-923.png', 0, 0, 0, 1, '2011-11-11', 12502, 0, '2017-01-01 00:00:00', 0), \
('The-Chris-Moyles-Show', '', 'http://96.31.83.86:8200/', '', 'The-Chris-Moyles-Show.png', 0, 0, 0, 1, '2011-11-11', 12503, 0, '2017-01-01 00:00:00', 0), \
('WGBW-1590', '', 'http://96.31.83.86:8200/', '', 'WGBW-1590.png', 0, 0, 0, 1, '2011-11-11', 12504, 0, '2017-01-01 00:00:00', 0), \
('AccuRadio-Love-Songs-Radio-Power-Ballads', '', 'http://96.31.83.86:8200/', 'http://www.accuradio.com/lovesongs/', '', 0, 0, 0, 1, '2011-11-11', 12505, 0, '2017-01-01 00:00:00', 0), \
('John-Lennon', '', 'http://96.31.83.86:8200/', '', 'John-Lennon.png', 0, 0, 0, 1, '2011-11-11', 12506, 0, '2017-01-01 00:00:00', 0), \
('Depressive-metal-rock-Radio', 'Depressive metal rock is a Channel which diffuses 24h/24. It is the intention of this Channel to offer an alternative from the public radio. <BR><BR>Depressive metal rock advantages: Targeted-style me', 'http://96.31.83.86:8200/', 'http://www.depressivemetalrockradio.net76.net/', 'Depressive-metal-rock-Radio.png', 0, 0, 0, 1, '2011-11-11', 12507, 0, '2017-01-01 00:00:00', 0), \
('The-Gatlin-Brothers', '', 'http://96.31.83.86:8200/', '', 'The-Gatlin-Brothers.png', 0, 0, 0, 1, '2011-11-11', 12508, 0, '2017-01-01 00:00:00', 0), \
('WQNX-1350', '', 'http://96.31.83.86:8200/', 'http://www.wqnx.net/', 'WQNX-1350.png', 0, 0, 0, 1, '2011-11-11', 12509, 0, '2017-01-01 00:00:00', 0), \
('WRJD-1410', '', 'http://96.31.83.86:8200/', 'http://www.1410wrjd.com/', 'WRJD-1410.png', 0, 0, 0, 1, '2011-11-11', 12510, 0, '2017-01-01 00:00:00', 0), \
('The-Stanley-Clarke-Band', '', 'http://96.31.83.86:8200/', '', 'The-Stanley-Clarke-Band.png', 0, 0, 0, 1, '2011-11-11', 12511, 0, '2017-01-01 00:00:00', 0), \
('Rosalia-de-Souza', '', 'http://96.31.83.86:8200/', '', 'Rosalia-de-Souza.png', 0, 0, 0, 1, '2011-11-11', 12512, 0, '2017-01-01 00:00:00', 0), \
('Star-Cambridge-1079', 'Playing Cambridge Greatest Hits...', 'http://96.31.83.86:8200/', 'http://www.star107.co.uk/', 'Star-Cambridge-1079.png', 0, 0, 0, 1, '2011-11-11', 12513, 0, '2017-01-01 00:00:00', 0), \
('KBLP-1051', '', 'http://96.31.83.86:8200/', 'http://www.kblpradio.com/', 'KBLP-1051.png', 0, 0, 0, 1, '2011-11-11', 12514, 0, '2017-01-01 00:00:00', 0), \
('The-Road-to-Reality', '', 'http://96.31.83.86:8200/', '', 'The-Road-to-Reality.png', 0, 0, 0, 1, '2011-11-11', 12515, 0, '2017-01-01 00:00:00', 0), \
('Aqua', '', 'http://96.31.83.86:8200/', '', 'Aqua.png', 0, 0, 0, 1, '2011-11-11', 12516, 0, '2017-01-01 00:00:00', 0), \
('MMA-Tonight', '', 'http://96.31.83.86:8200/', '', 'MMA-Tonight.png', 0, 0, 0, 1, '2011-11-11', 12517, 0, '2017-01-01 00:00:00', 0), \
('979X-Third-Shift', '', 'http://96.31.83.86:8200/', '', '979X-Third-Shift.png', 0, 0, 0, 1, '2011-11-11', 12518, 0, '2017-01-01 00:00:00', 0), \
('Puff-Daddy', '', 'http://96.31.83.86:8200/', '', 'Puff-Daddy.png', 0, 0, 0, 1, '2011-11-11', 12519, 0, '2017-01-01 00:00:00', 0), \
('Maximum-1037', '', 'http://96.31.83.86:8200/', 'http://www.maximum.ru/', 'Maximum-1037.png', 0, 0, 0, 1, '2011-11-11', 12520, 0, '2017-01-01 00:00:00', 0), \
('The-River-1045', 'WRVR is your at work station with your favorite music!', 'http://96.31.83.86:8200/', 'http://www.wrvr.com/', 'The-River-1045.png', 0, 0, 0, 1, '2011-11-11', 12521, 0, '2017-01-01 00:00:00', 0), \
('1CLUBFM--Reggaeton', 'Today&#39;s Reggaeton stars 24/7', 'http://96.31.83.86:8200/', 'http://www.1club.fm/', '1CLUBFM--Reggaeton.png', 0, 0, 1, 1, '2011-11-11', 12522, 0, '2011-10-17 23:39:53', 0), \
('RTFM', '', 'http://96.31.83.86:8200/', 'http://www.rtfm-radio.com/', 'RTFM.png', 0, 0, 0, 1, '2011-11-11', 12523, 0, '2017-01-01 00:00:00', 0), \
('Love-Unlimited-Orchestra', '', 'http://96.31.83.86:8200/', '', 'Love-Unlimited-Orchestra.png', 0, 0, 0, 1, '2011-11-11', 12524, 0, '2017-01-01 00:00:00', 0), \
('Jonny-Lang', '', 'http://96.31.83.86:8200/', '', 'Jonny-Lang.png', 0, 0, 0, 1, '2011-11-11', 12525, 0, '2017-01-01 00:00:00', 0), \
('KRXO-1077', '', 'http://96.31.83.86:8200/', 'http://www.krxo.com/', 'KRXO-1077.png', 0, 0, 0, 1, '2011-11-11', 12526, 0, '2017-01-01 00:00:00', 0), \
('WIKI-953', '', 'http://96.31.83.86:8200/', '', 'WIKI-953.png', 0, 0, 0, 1, '2011-11-11', 12527, 0, '2017-01-01 00:00:00', 0), \
('Broadways-Biggest-Hits', '', 'http://96.31.83.86:8200/', '', 'Broadways-Biggest-Hits.png', 0, 0, 0, 1, '2011-11-11', 12528, 0, '2017-01-01 00:00:00', 0), \
('B937', '', 'http://96.31.83.86:8200/', 'http://www.brookingsradio.com/pages/8777876.php', 'B937.png', 0, 0, 0, 1, '2011-11-11', 12529, 0, '2017-01-01 00:00:00', 0), \
('Psychedelic-Jukebox', 'The best of 60 Psychedelic and Garage Band music with Surf tunes your Host, Kevin Anthony', 'http://96.31.83.86:8200/', 'http://www.psychedelicjukebox.com/', 'Psychedelic-Jukebox.png', 0, 0, 0, 1, '2011-11-11', 12530, 0, '2017-01-01 00:00:00', 0), \
('My-1043', '', 'http://96.31.83.86:8200/', 'http://www.wczy.net/', 'My-1043.png', 0, 0, 0, 1, '2011-11-11', 12531, 0, '2017-01-01 00:00:00', 0), \
('Chaka-Khan', '', 'http://96.31.83.86:8200/', '', 'Chaka-Khan.png', 0, 0, 0, 1, '2011-11-11', 12533, 0, '2017-01-01 00:00:00', 0), \
('KOSS-1380', 'News, Talk, Call-In', 'http://96.31.83.86:8200/', 'http://www.newstalk1380.com/', 'KOSS-1380.png', 0, 0, 0, 1, '2011-11-11', 12534, 0, '2017-01-01 00:00:00', 0), \
('GenX-Radio-1003', '', 'http://96.31.83.86:8200/', 'http://www.genx1003.com/', 'GenX-Radio-1003.png', 0, 0, 0, 1, '2011-11-11', 12535, 0, '2017-01-01 00:00:00', 0), \
('FAN-Motorsports', '', 'http://96.31.83.86:8200/', '', 'FAN-Motorsports.png', 0, 0, 0, 1, '2011-11-11', 12536, 0, '2017-01-01 00:00:00', 0), \
('101-ESPN-1011', '', 'http://96.31.83.86:8200/', 'http://www.101espn.com/', '101-ESPN-1011.png', 0, 0, 0, 1, '2011-11-11', 12537, 0, '2017-01-01 00:00:00', 0), \
('K92-923', '', 'http://96.31.83.86:8200/', 'http://www.k92radio.com/', 'K92-923.png', 0, 0, 0, 1, '2011-11-11', 12538, 0, '2017-01-01 00:00:00', 0), \
('George-Benson', '', 'http://96.31.83.86:8200/', '', 'George-Benson.png', 0, 0, 0, 1, '2011-11-11', 12539, 0, '2017-01-01 00:00:00', 0), \
('Radio-Lux-Lviv-1047', '', 'http://96.31.83.86:8200/', 'http://www.lux.fm/', 'Radio-Lux-Lviv-1047.png', 0, 0, 0, 1, '2011-11-11', 12540, 0, '2017-01-01 00:00:00', 0), \
('The-Tide-923', '', 'http://96.31.83.86:8200/', 'http://www.tideradio.com/', 'The-Tide-923.png', 0, 0, 0, 1, '2011-11-11', 12541, 0, '2017-01-01 00:00:00', 0), \
('KIQI-1010', '', 'http://96.31.83.86:8200/', 'http://www.kiqi1010am.com/', 'KIQI-1010.png', 0, 0, 0, 1, '2011-11-11', 12542, 0, '2017-01-01 00:00:00', 0), \
('WNAM-1280', '', 'http://96.31.83.86:8200/', 'http://www.1280wnam.com/', 'WNAM-1280.png', 0, 0, 0, 1, '2011-11-11', 12543, 0, '2017-01-01 00:00:00', 0), \
('Kool-98-981', '', 'http://96.31.83.86:8200/', 'http://www.98kool.com/', 'Kool-98-981.png', 0, 0, 0, 1, '2011-11-11', 12544, 0, '2017-01-01 00:00:00', 0), \
('WMIX-FM-941', '', 'http://96.31.83.86:8200/', 'http://www.mywithersradio.com/wmix', 'WMIX-FM-941.png', 0, 0, 0, 1, '2011-11-11', 12545, 0, '2017-01-01 00:00:00', 0), \
('Rookwood-Sound-945', '', 'http://96.31.83.86:8200/', 'http://www.rookwoodsound.co.uk/', 'Rookwood-Sound-945.png', 0, 0, 0, 1, '2011-11-11', 12546, 0, '2017-01-01 00:00:00', 0), \
('Blues-on-Blues', '', 'http://96.31.83.86:8200/', '', 'Blues-on-Blues.png', 0, 0, 0, 1, '2011-11-11', 12547, 0, '2017-01-01 00:00:00', 0), \
('Gretchen-Parlato', '', 'http://96.31.83.86:8200/', '', 'Gretchen-Parlato.png', 0, 0, 0, 1, '2011-11-11', 12548, 0, '2017-01-01 00:00:00', 0), \
('All-Things-Jazz', '', 'http://96.31.83.86:8200/', '', 'All-Things-Jazz.png', 0, 0, 1, 1, '2011-11-11', 12549, 0, '2011-10-15 20:12:04', 0), \
('Korn', '', 'http://96.31.83.86:8200/', '', 'Korn.png', 0, 0, 0, 1, '2011-11-11', 12550, 0, '2017-01-01 00:00:00', 0), \
('1073-WBBL', '', 'http://96.31.83.86:8200/', 'http://www.wbbl.com/', '1073-WBBL.png', 0, 0, 0, 1, '2011-11-11', 12551, 0, '2017-01-01 00:00:00', 0), \
('KQAD-800', '', 'http://96.31.83.86:8200/', '', 'KQAD-800.png', 0, 0, 0, 1, '2011-11-11', 12552, 0, '2017-01-01 00:00:00', 0), \
('Mix-1065', '', 'http://96.31.83.86:8200/', 'http://www.kezr.com/', 'Mix-1065.png', 0, 0, 0, 1, '2011-11-11', 12553, 0, '2017-01-01 00:00:00', 0), \
('Annie-Nightingale', '', 'http://96.31.83.86:8200/', '', 'Annie-Nightingale.png', 0, 0, 0, 1, '2011-11-11', 12554, 0, '2017-01-01 00:00:00', 0), \
('KTOK-1000', '', 'http://96.31.83.86:8200/', 'http://www.ktok.com/', 'KTOK-1000.png', 0, 0, 0, 1, '2011-11-11', 12555, 0, '2017-01-01 00:00:00', 0), \
('Roskilde-Festival-Radio-923', '', 'http://96.31.83.86:8200/', 'http://www.festivalradio.dk/', 'Roskilde-Festival-Radio-923.png', 0, 0, 0, 1, '2011-11-11', 12556, 0, '2017-01-01 00:00:00', 0), \
('i93-933', '', 'http://96.31.83.86:8200/', 'http://www.i93hits.com', 'i93-933.png', 0, 0, 0, 1, '2011-11-11', 12557, 0, '2017-01-01 00:00:00', 0), \
('OK-Go', '', 'http://96.31.83.86:8200/', '', 'OK-Go.png', 0, 0, 0, 1, '2011-11-11', 12558, 0, '2017-01-01 00:00:00', 0), \
('Radio-GAY-Rainbow', '', 'http://96.31.83.86:8200/', 'http://radio.gayrainbow.ru/', 'Radio-GAY-Rainbow.png', 0, 0, 0, 1, '2011-11-11', 12560, 0, '2017-01-01 00:00:00', 0), \
('The-Smashing-Pumpkins', '', 'http://96.31.83.86:8200/', '', 'The-Smashing-Pumpkins.png', 0, 0, 0, 1, '2011-11-11', 12561, 0, '2017-01-01 00:00:00', 0), \
('Kix-1057', '', 'http://96.31.83.86:8200/', 'http://www.kxkx.com/', 'Kix-1057.png', 0, 0, 0, 1, '2011-11-11', 12562, 0, '2017-01-01 00:00:00', 0), \
('Power-Radio-Classic-Soul', '', 'http://96.31.83.86:8200/', 'http://www.powerhitz.com/', 'Power-Radio-Classic-Soul.png', 0, 0, 0, 1, '2011-11-11', 12563, 0, '2017-01-01 00:00:00', 0), \
('KGSR-933', '', 'http://96.31.83.86:8200/', 'http://www.kgsr.com/', 'KGSR-933.png', 0, 0, 0, 1, '2011-11-11', 12564, 0, '2017-01-01 00:00:00', 0), \
('WNIO-1390', '', 'http://96.31.83.86:8200/', 'http://www.wnio.com/', 'WNIO-1390.png', 0, 0, 0, 1, '2011-11-11', 12565, 0, '2017-01-01 00:00:00', 0), \
('Bear-Hands', '', 'http://96.31.83.86:8200/', '', 'Bear-Hands.png', 0, 0, 0, 1, '2011-11-11', 12566, 0, '2017-01-01 00:00:00', 0), \
('Five-for-Fighting', '', 'http://96.31.83.86:8200/', '', 'Five-for-Fighting.png', 0, 0, 0, 1, '2011-11-11', 12567, 0, '2017-01-01 00:00:00', 0), \
('Radio-Dukagjini-997', '', 'http://96.31.83.86:8200/', 'http://www.radio-dukagjini.com/', 'Radio-Dukagjini-997.png', 0, 0, 0, 1, '2011-11-11', 12568, 0, '2017-01-01 00:00:00', 0), \
('WCDR-Morning-Praise', '', 'http://96.31.83.86:8200/', '', 'WCDR-Morning-Praise.png', 0, 0, 0, 1, '2011-11-11', 12569, 0, '2017-01-01 00:00:00', 0), \
('AccuRadio-AccuCountry-New-Country-Hits', '', 'http://96.31.83.86:8200/', 'http://www.accuradio.com/country/', '', 0, 0, 0, 1, '2011-11-11', 12570, 0, '2017-01-01 00:00:00', 0), \
('Christian-Nash', '', 'http://96.31.83.86:8200/', '', 'Christian-Nash.png', 0, 0, 0, 1, '2011-11-11', 12571, 0, '2017-01-01 00:00:00', 0), \
('Frightened-Rabbit', '', 'http://96.31.83.86:8200/', '', 'Frightened-Rabbit.png', 0, 0, 0, 1, '2011-11-11', 12572, 0, '2017-01-01 00:00:00', 0), \
('Kids-Corner', '', 'http://96.31.83.86:8200/', '', 'Kids-Corner.png', 0, 0, 0, 1, '2011-11-11', 12573, 0, '2017-01-01 00:00:00', 0), \
('Mix-Up-Exclusives', '', 'http://96.31.83.86:8200/', '', 'Mix-Up-Exclusives.png', 0, 0, 0, 1, '2011-11-11', 12574, 0, '2017-01-01 00:00:00', 0), \
('Sports-Talk', '', 'http://96.31.83.86:8200/', '', 'Sports-Talk.png', 0, 0, 0, 1, '2011-11-11', 12575, 0, '2017-01-01 00:00:00', 0), \
('CBC-Radio-3-Pop', '', 'http://96.31.83.86:8200/', 'http://radio3.cbc.ca/', 'CBC-Radio-3-Pop.png', 0, 0, 0, 1, '2011-11-11', 12576, 0, '2017-01-01 00:00:00', 0), \
('RPR-1-1015', 'Das Gute von gestern mit dem Besten von heute', 'http://96.31.83.86:8200/', 'http://www.rpr1.de/', 'RPR-1-1015.png', 0, 0, 0, 1, '2011-11-11', 12577, 0, '2017-01-01 00:00:00', 0), \
('The-Big-Dawg-999', '', 'http://96.31.83.86:8200/', 'http://www.wvlc.com/', 'The-Big-Dawg-999.png', 0, 0, 0, 1, '2011-11-11', 12578, 0, '2017-01-01 00:00:00', 0), \
('WJST-1280', '', 'http://96.31.83.86:8200/', '', 'WJST-1280.png', 0, 0, 0, 1, '2011-11-11', 12579, 0, '2017-01-01 00:00:00', 0), \
('Annie-Lennox', '', 'http://96.31.83.86:8200/', '', 'Annie-Lennox.png', 0, 0, 0, 1, '2011-11-11', 12580, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-Glory', '', 'http://96.31.83.86:8200/', '', 'Sunday-Morning-Glory.png', 0, 0, 0, 1, '2011-11-11', 12581, 0, '2017-01-01 00:00:00', 0), \
('Imagen-en-Ciencia', '', 'http://96.31.83.86:8200/', '', 'Imagen-en-Ciencia.png', 0, 0, 0, 1, '2011-11-11', 12582, 0, '2017-01-01 00:00:00', 0), \
('Senior-Bowl-Locker-Room', '', 'http://96.31.83.86:8200/', '', 'Senior-Bowl-Locker-Room.png', 0, 0, 0, 1, '2011-11-11', 12583, 0, '2017-01-01 00:00:00', 0), \
('T-100-1003', '', 'http://96.31.83.86:8200/', 'http://www.wclt.com/fm/index.cfm', 'T-100-1003.png', 0, 0, 0, 1, '2011-11-11', 12584, 0, '2017-01-01 00:00:00', 0), \
('Gold-905-FM', '', 'http://96.31.83.86:8200/', 'http://www.gold90.sg/', 'Gold-905-FM.png', 0, 0, 0, 1, '2011-11-11', 12585, 0, '2017-01-01 00:00:00', 0), \
('French-Open-Radio-(French)', '', 'http://96.31.83.86:8200/', 'http://www.rolandgarros.com/en_FR/multimedia/radio/index.html', 'French-Open-Radio-(French).png', 0, 0, 0, 1, '2011-11-11', 12586, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Dance-Hits', 'The best dance hits, club hits, and the hottest pop crossovers and remixes. Get the party started and keep the beat going with all these dance favorites.', 'http://96.31.83.86:8200/', 'http://www.radioio.com/channels/dance-hits', 'RadioIO-Dance-Hits.png', 0, 0, 0, 1, '2011-11-11', 12587, 0, '2017-01-01 00:00:00', 0), \
('The-Nat-King-Cole-Trio', '', 'http://96.31.83.86:8200/', '', 'The-Nat-King-Cole-Trio.png', 0, 0, 0, 1, '2011-11-11', 12588, 0, '2017-01-01 00:00:00', 0), \
('Andrew-Bird', '', 'http://96.31.83.86:8200/', '', 'Andrew-Bird.png', 0, 0, 0, 1, '2011-11-11', 12589, 0, '2017-01-01 00:00:00', 0), \
('Spawn-of-Possession', '', 'http://96.31.83.86:8200/', '', 'Spawn-of-Possession.png', 0, 0, 0, 1, '2011-11-11', 12590, 0, '2017-01-01 00:00:00', 0), \
('I-95-955', 'Heritage Rock Station for over 25 years!  Features Classic and New Rock', 'http://96.31.83.86:8200/', 'http://www.rockofsavannah.net/', 'I-95-955.png', 0, 0, 0, 1, '2011-11-11', 12591, 0, '2017-01-01 00:00:00', 0), \
('1-Radio-Latino', 'Hier erwartet euch mein Internetstream 24 Stunden alles rund um Salsa', 'http://96.31.83.86:8200/', 'http://radiolatinostream.de.to', '1-Radio-Latino.png', 0, 0, 0, 1, '2011-11-11', 12592, 0, '2017-01-01 00:00:00', 0), \
('KRSA-580', '', 'http://96.31.83.86:8200/', 'http://www.krsa.net/', 'KRSA-580.png', 0, 0, 0, 1, '2011-11-11', 12593, 0, '2017-01-01 00:00:00', 0), \
('The-Valley-1009', '', 'http://96.31.83.86:8200/', 'http://www.wvly.com/', 'The-Valley-1009.png', 0, 0, 0, 1, '2011-11-11', 12594, 0, '2017-01-01 00:00:00', 0), \
('House-Music-Radio', 'Deep House, Soulful House, Funky House 24/7', 'http://96.31.83.86:8200/', 'http://beta.housemusicradio.pl/', 'House-Music-Radio.png', 0, 0, 0, 1, '2011-11-11', 12595, 0, '2017-01-01 00:00:00', 0), \
('Mara-Isabel', '', 'http://96.31.83.86:8200/', '', 'Mara-Isabel.png', 0, 0, 0, 1, '2011-11-11', 12596, 0, '2017-01-01 00:00:00', 0), \
('Cow-97-971', '', 'http://96.31.83.86:8200/', 'http://www.cow97.com/', 'Cow-97-971.png', 0, 0, 0, 1, '2011-11-11', 12598, 0, '2017-01-01 00:00:00', 0), \
('KMND-1510', '', 'http://96.31.83.86:8200/', 'http://www.kmnd.com/', 'KMND-1510.png', 0, 0, 0, 1, '2011-11-11', 12599, 0, '2017-01-01 00:00:00', 0), \
('Rock-1045', '', 'http://96.31.83.86:8200/', 'http://www.kdot.com/', 'Rock-1045.png', 0, 0, 0, 1, '2011-11-11', 12600, 0, '2017-01-01 00:00:00', 0), \
('BAGeL-Radio', 'Stay on top of the indie rock scene w/ this 24/7 Internet station.', 'http://96.31.83.86:8200/', 'http://bagelradio.com', 'BAGeL-Radio.png', 0, 0, 0, 1, '2011-11-11', 12601, 0, '2017-01-01 00:00:00', 0), \
('Marduk', '', 'http://96.31.83.86:8200/', '', 'Marduk.png', 0, 0, 0, 1, '2011-11-11', 12602, 0, '2017-01-01 00:00:00', 0), \
('Mod', '', 'http://96.31.83.86:8200/', '', 'Mod.png', 0, 0, 0, 1, '2011-11-11', 12603, 0, '2017-01-01 00:00:00', 0), \
('Donna-Summer', '', 'http://96.31.83.86:8200/', '', 'Donna-Summer.png', 0, 0, 0, 1, '2011-11-11', 12604, 0, '2017-01-01 00:00:00', 0), \
('Dashboard-Confessional', '', 'http://96.31.83.86:8200/', '', 'Dashboard-Confessional.png', 0, 0, 0, 1, '2011-11-11', 12605, 0, '2017-01-01 00:00:00', 0), \
('French-Open-Radio-(English)', 'Radio Roland Garros is the ideal way to catch all the action of the tournament, from the first point to the last, just as if you were in the stadium! The station is broadcast on www.rolandgarros.com a', 'http://96.31.83.86:8200/', 'http://www.rolandgarros.com/en_FR/multimedia/radio/index.html', 'French-Open-Radio-(English).png', 0, 0, 0, 1, '2011-11-11', 12606, 0, '2017-01-01 00:00:00', 0), \
('Radio-Raja-Yoga', 'Spiritual lectures, meditations, and meditative music.', 'http://96.31.83.86:8200/', 'http://www.raja-yoga.net/', 'Radio-Raja-Yoga.png', 0, 0, 0, 1, '2011-11-11', 12607, 0, '2017-01-01 00:00:00', 0), \
('Big-Blue-Swing', 'Big Blue Swing.com, features the best swing and blues music available.', 'http://96.31.83.86:8200/', 'http://www.bigblueswing.com/', 'Big-Blue-Swing.png', 0, 0, 0, 1, '2011-11-11', 12608, 0, '2017-01-01 00:00:00', 0), \
('Portofino-Network', '', 'http://96.31.83.86:8200/', 'http://radioportofino.blogspot.com/', 'Portofino-Network.png', 0, 0, 0, 1, '2011-11-11', 12609, 0, '2017-01-01 00:00:00', 0), \
('On-Broken-Wings', '', 'http://96.31.83.86:8200/', '', 'On-Broken-Wings.png', 0, 0, 0, 1, '2011-11-11', 12610, 0, '2017-01-01 00:00:00', 0), \
('KSTP-1500', '', 'http://96.31.83.86:8200/', 'http://1500espn.com/', 'KSTP-1500.png', 0, 0, 0, 1, '2011-11-11', 12611, 0, '2017-01-01 00:00:00', 0);";


const char *radio_station_setupsql39="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('CNN', 'The latest, breaking news with on-the-spot coverage and up-to-the-minute updates.', 'http://96.31.83.86:8200/', 'http://www.cnn.com/', 'CNN.png', 0, 0, 0, 1, '2011-11-11', 12612, 0, '2017-01-01 00:00:00', 0), \
('973-Forth-One', '', 'http://96.31.83.86:8200/', 'http://www.forthone.com/', '973-Forth-One.png', 0, 0, 0, 1, '2011-11-11', 12613, 0, '2017-01-01 00:00:00', 0), \
('3-RRR-1027', 'Three Triple R is a listener-funded community radio station where almost all the broadcasters are volunteers who work alongside a small paid staff. Around 70% of programming is music based with both g', 'http://96.31.83.86:8200/', 'http://www.rrr.org.au/', '3-RRR-1027.png', 0, 0, 0, 1, '2011-11-11', 12614, 0, '2017-01-01 00:00:00', 0), \
('Sunshine-Radio-1062', 'Sunshine Radio is a radio station which broadcasts to Herefordshire and Monmouthshire from its studios in Hereford, England. It is operated by Murfin Media Ltd, and previously by Laser Broadcasting.		', 'http://96.31.83.86:8200/', 'http://sunshineradio.moonfruit.com/', 'Sunshine-Radio-1062.png', 0, 0, 0, 1, '2011-11-11', 12615, 0, '2017-01-01 00:00:00', 0), \
('Fay-Lovsky', '', 'http://96.31.83.86:8200/', '', 'Fay-Lovsky.png', 0, 0, 0, 1, '2011-11-11', 12616, 0, '2017-01-01 00:00:00', 0), \
('WOWO-1190', '', 'http://96.31.83.86:8200/', 'http://www.wowo.com/', 'WOWO-1190.png', 0, 0, 0, 1, '2011-11-11', 12617, 0, '2017-01-01 00:00:00', 0), \
('The-Fox-997', '', 'http://96.31.83.86:8200/', 'http://www.wrfx.com/', 'The-Fox-997.png', 0, 0, 0, 1, '2011-11-11', 12618, 0, '2017-01-01 00:00:00', 0), \
('Indianapolis-Colts-Post-Game-Show', '', 'http://96.31.83.86:8200/', '', 'Indianapolis-Colts-Post-Game-Show.png', 0, 0, 0, 1, '2011-11-11', 12619, 0, '2017-01-01 00:00:00', 0), \
('Hevia', '', 'http://96.31.83.86:8200/', '', 'Hevia.png', 0, 0, 0, 1, '2011-11-11', 12620, 0, '2017-01-01 00:00:00', 0), \
('KSPI-780', '', 'http://96.31.83.86:8200/', 'http://www.stillwaterradio.net/', 'KSPI-780.png', 0, 0, 0, 1, '2011-11-11', 12621, 0, '2017-01-01 00:00:00', 0), \
('The-Whitefield-Brothers', '', 'http://96.31.83.86:8200/', '', 'The-Whitefield-Brothers.png', 0, 0, 0, 1, '2011-11-11', 12622, 0, '2017-01-01 00:00:00', 0), \
('The-Edgar-Winter-Group', '', 'http://96.31.83.86:8200/', '', 'The-Edgar-Winter-Group.png', 0, 0, 0, 1, '2011-11-11', 12623, 0, '2017-01-01 00:00:00', 0), \
('Lebo-M', '', 'http://96.31.83.86:8200/', '', 'Lebo-M.png', 0, 0, 0, 1, '2011-11-11', 12624, 0, '2017-01-01 00:00:00', 0), \
('KZSB-1290', '', 'http://96.31.83.86:8200/', 'http://www.newspress.com/', 'KZSB-1290.png', 0, 0, 0, 1, '2011-11-11', 12625, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Action-Sports', '', 'http://96.31.83.86:8200/', '', 'ESPN-Action-Sports.png', 0, 0, 0, 1, '2011-11-11', 12626, 0, '2017-01-01 00:00:00', 0), \
('Anberlin', '', 'http://96.31.83.86:8200/', '', 'Anberlin.png', 0, 0, 0, 1, '2011-11-11', 12627, 0, '2017-01-01 00:00:00', 0), \
('ESPN-590', '', 'http://96.31.83.86:8200/', 'http://www.espn590.com/', 'ESPN-590.png', 0, 0, 0, 1, '2011-11-11', 12628, 0, '2017-01-01 00:00:00', 0), \
('Arkansas-Razorbacks-Basketball', '', 'http://96.31.83.86:8200/', '', 'Arkansas-Razorbacks-Basketball.png', 0, 0, 0, 1, '2011-11-11', 12629, 0, '2017-01-01 00:00:00', 0), \
('Dictionary', '', 'http://96.31.83.86:8200/', '', 'Dictionary.png', 0, 0, 0, 1, '2011-11-11', 12630, 0, '2017-01-01 00:00:00', 0), \
('AccuRadio-Check-the-Rhyme-Old-School', '', 'http://96.31.83.86:8200/', 'http://www.accuradio.com/hiphop/', 'AccuRadio-Check-the-Rhyme-Old-School.png', 0, 0, 0, 1, '2011-11-11', 12631, 0, '2017-01-01 00:00:00', 0), \
('KZ-106-1065', '', 'http://96.31.83.86:8200/', 'http://www.wskz.com/', 'KZ-106-1065.png', 0, 0, 0, 1, '2011-11-11', 12632, 0, '2017-01-01 00:00:00', 0), \
('WMOA-1490', '', 'http://96.31.83.86:8200/', 'http://www.wmoa1490.com/', 'WMOA-1490.png', 0, 0, 0, 1, '2011-11-11', 12633, 0, '2017-01-01 00:00:00', 0), \
('WWTK-730', '', 'http://96.31.83.86:8200/', 'http://www.cohanradiogroup.com/', 'WWTK-730.png', 0, 0, 0, 1, '2011-11-11', 12634, 0, '2017-01-01 00:00:00', 0), \
('6PR-882', '', 'http://96.31.83.86:8200/', 'http://www.6pr.com.au/', '6PR-882.png', 0, 0, 0, 1, '2011-11-11', 12635, 0, '2017-01-01 00:00:00', 0), \
('Ratt', '', 'http://96.31.83.86:8200/', '', 'Ratt.png', 0, 0, 0, 1, '2011-11-11', 12636, 0, '2017-01-01 00:00:00', 0), \
('Cameo', '', 'http://96.31.83.86:8200/', '', 'Cameo.png', 0, 0, 0, 1, '2011-11-11', 12637, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Blues', 'Blues music that started in the 20s and that keeps making an impact today. It&#39;s a musical and geographic journey, from the Mississippi Delta, to Chicago, Texas and beyond.', 'http://96.31.83.86:8200/', 'http://www.radioio.com/channels/blues', 'RadioIO-Blues.png', 0, 0, 0, 1, '2011-11-11', 12638, 0, '2017-01-01 00:00:00', 0), \
('KMHT-1450', 'KMHT-FM Radio - the heartbeat of East Texas - www.kmhtradio.com.', 'http://96.31.83.86:8200/', 'http://www.kmhtradio.com/', 'KMHT-1450.png', 0, 0, 0, 1, '2011-11-11', 12639, 0, '2017-01-01 00:00:00', 0), \
('World-Cafe', '', 'http://96.31.83.86:8200/', '', 'World-Cafe.png', 0, 0, 0, 1, '2011-11-11', 12640, 0, '2017-01-01 00:00:00', 0), \
('Moonjam', '', 'http://96.31.83.86:8200/', '', 'Moonjam.png', 0, 0, 0, 1, '2011-11-11', 12641, 0, '2017-01-01 00:00:00', 0), \
('Bay-City-Rollers', '', 'http://96.31.83.86:8200/', '', 'Bay-City-Rollers.png', 0, 0, 1, 1, '2011-11-11', 12642, 0, '2011-10-14 16:02:49', 0), \
('Sunday-Magazine', '', 'http://96.31.83.86:8200/', '', 'Sunday-Magazine.png', 0, 0, 0, 1, '2011-11-11', 12643, 0, '2017-01-01 00:00:00', 0), \
('WCLD-FM-1039', '', 'http://96.31.83.86:8200/', '', 'WCLD-FM-1039.png', 0, 0, 0, 1, '2011-11-11', 12644, 0, '2017-01-01 00:00:00', 0), \
('Ron-Carter', '', 'http://96.31.83.86:8200/', '', 'Ron-Carter.png', 0, 0, 0, 1, '2011-11-11', 12645, 0, '2017-01-01 00:00:00', 0), \
('The-Bouncing-Souls', '', 'http://96.31.83.86:8200/', '', 'The-Bouncing-Souls.png', 0, 0, 0, 1, '2011-11-11', 12646, 0, '2017-01-01 00:00:00', 0), \
('Scott-Linden-Outdoors', '', 'http://96.31.83.86:8200/', '', 'Scott-Linden-Outdoors.png', 0, 0, 0, 1, '2011-11-11', 12647, 0, '2017-01-01 00:00:00', 0), \
('stellastarr', '', 'http://96.31.83.86:8200/', '', 'stellastarr.png', 0, 0, 0, 1, '2011-11-11', 12648, 0, '2017-01-01 00:00:00', 0), \
('Generationals', '', 'http://96.31.83.86:8200/', '', 'Generationals.png', 0, 0, 0, 1, '2011-11-11', 12649, 0, '2017-01-01 00:00:00', 0), \
('Newstalk-1260-Weekend', '', 'http://96.31.83.86:8200/', '', 'Newstalk-1260-Weekend.png', 0, 0, 0, 1, '2011-11-11', 12650, 0, '2017-01-01 00:00:00', 0), \
('ALEX-Offener-Kanal-Berlin-926', 'Ereignisse aus den Bereichen Politik, Kultur und Gesellschaft finden bei ALEX ihre Darstellung. Mit ALEX k&#246;nnen Menschen Medieninhalte gleichzeitig erleben und gestalten.', 'http://96.31.83.86:8200/', 'http://www.alex-berlin.de', 'ALEX-Offener-Kanal-Berlin-926.png', 0, 0, 0, 1, '2011-11-11', 12651, 0, '2017-01-01 00:00:00', 0), \
('WGRR-1035', '', 'http://96.31.83.86:8200/', 'http://www.wgrr.com/', 'WGRR-1035.png', 0, 0, 0, 1, '2011-11-11', 12652, 0, '2017-01-01 00:00:00', 0), \
('Jazzworks', '', 'http://96.31.83.86:8200/', '', 'Jazzworks.png', 0, 0, 0, 1, '2011-11-11', 12653, 0, '2017-01-01 00:00:00', 0), \
('Know-The-Truth', '', 'http://96.31.83.86:8200/', '', 'Know-The-Truth.png', 0, 0, 0, 1, '2011-11-11', 12654, 0, '2017-01-01 00:00:00', 0), \
('Bright-Star-100-1001', 'Bartlesville Radio is the News, Sports, and Weather Source for Washington, Osage, and Nowata Counties.', 'http://96.31.83.86:8200/', 'http://www.bartlesvilleradio.com/', 'Bright-Star-100-1001.png', 0, 0, 0, 1, '2011-11-11', 12655, 0, '2017-01-01 00:00:00', 0), \
('Zapp--Roger', '', 'http://96.31.83.86:8200/', '', 'Zapp--Roger.png', 0, 0, 0, 1, '2011-11-11', 12656, 0, '2017-01-01 00:00:00', 0), \
('Joe-Arroyo', '', 'http://96.31.83.86:8200/', '', 'Joe-Arroyo.png', 0, 0, 0, 1, '2011-11-11', 12657, 0, '2017-01-01 00:00:00', 0), \
('Braid', '', 'http://96.31.83.86:8200/', '', 'Braid.png', 0, 0, 0, 1, '2011-11-11', 12658, 0, '2017-01-01 00:00:00', 0), \
('Way-FM-1019', '', 'http://96.31.83.86:8200/', 'http://kxwa.wayfm.com/', 'Way-FM-1019.png', 0, 0, 0, 1, '2011-11-11', 12659, 0, '2017-01-01 00:00:00', 0), \
('WJOL-1340', '', 'http://96.31.83.86:8200/', 'http://www.wjol.com/', 'WJOL-1340.png', 0, 0, 0, 1, '2011-11-11', 12660, 0, '2017-01-01 00:00:00', 0), \
('945-KSMB', '', 'http://96.31.83.86:8200/', 'http://www.ksmb.com/', '945-KSMB.png', 0, 0, 0, 1, '2011-11-11', 12661, 0, '2017-01-01 00:00:00', 0), \
('Relax-FM-908', '', 'http://96.31.83.86:8200/', 'http://www.relax-fm.ru/', 'Relax-FM-908.png', 0, 0, 0, 1, '2011-11-11', 12662, 0, '2017-01-01 00:00:00', 0), \
('SomaFM-PopTron', 'Indie dance rock. Less guitars, more synths. Catchy electro-pop songs from artists like Cut Copy, Datarock, MGMT and Hot Chip. You can&#39;t help but like it. Only from SomaFM.com.', 'http://96.31.83.86:8200/', 'http://somafm.com/', 'SomaFM-PopTron.png', 0, 0, 0, 1, '2011-11-11', 12663, 0, '2017-01-01 00:00:00', 0), \
('Turisas', '', 'http://96.31.83.86:8200/', '', 'Turisas.png', 0, 0, 0, 1, '2011-11-11', 12664, 0, '2017-01-01 00:00:00', 0), \
('Seventies-Saturday', '', 'http://96.31.83.86:8200/', '', 'Seventies-Saturday.png', 0, 0, 0, 1, '2011-11-11', 12665, 0, '2017-01-01 00:00:00', 0), \
('Studentradioen-i-Bergen-1078', '', 'http://96.31.83.86:8200/', 'http://www.srib.no/', 'Studentradioen-i-Bergen-1078.png', 0, 0, 0, 1, '2011-11-11', 12666, 0, '2017-01-01 00:00:00', 0), \
('Van-Halen', '', 'http://96.31.83.86:8200/', '', 'Van-Halen.png', 0, 0, 0, 1, '2011-11-11', 12667, 0, '2017-01-01 00:00:00', 0), \
('Alicia-Keys', '', 'http://96.31.83.86:8200/', '', 'Alicia-Keys.png', 0, 0, 1, 1, '2011-11-11', 12668, 0, '2011-10-15 20:11:12', 0), \
('KSDW-889', '', 'http://96.31.83.86:8200/', 'http://www.ksdwradio.com/', 'KSDW-889.png', 0, 0, 0, 1, '2011-11-11', 12669, 0, '2017-01-01 00:00:00', 0), \
('Gay-Radio', 'An odd but fun mix of music from somewhere over the rainbow.', 'http://96.31.83.86:8200/', 'http://gayradio.ru/', 'Gay-Radio.png', 0, 0, 0, 1, '2011-11-11', 12670, 0, '2017-01-01 00:00:00', 0), \
('Rock-1069', '', 'http://96.31.83.86:8200/', 'http://www.wrqk.com/', 'Rock-1069.png', 0, 0, 0, 1, '2011-11-11', 12671, 0, '2017-01-01 00:00:00', 0), \
('Radio-Aura-1054', '', 'http://96.31.83.86:8200/', 'http://www.radioaura.dk', 'Radio-Aura-1054.png', 0, 0, 0, 1, '2011-11-11', 12672, 0, '2017-01-01 00:00:00', 0), \
('Sunlight', '', 'http://96.31.83.86:8200/', '', 'Sunlight.png', 0, 0, 0, 1, '2011-11-11', 12673, 0, '2017-01-01 00:00:00', 0), \
('Josh-Turner', '', 'http://96.31.83.86:8200/', '', 'Josh-Turner.png', 0, 0, 0, 1, '2011-11-11', 12674, 0, '2017-01-01 00:00:00', 0), \
('WKLB-FM-1025', 'Country 102.5 WKLB is Boston&#39;s &quot;10 in a Row Country Station&quot; and features contemporary country music. The radio station is very music intensive. TSL is the name of the game. The audience', 'http://96.31.83.86:8200/', 'http://www.wklb.com/', 'WKLB-FM-1025.png', 0, 0, 0, 1, '2011-11-11', 12675, 0, '2017-01-01 00:00:00', 0), \
('Das-Racist', '', 'http://96.31.83.86:8200/', '', 'Das-Racist.png', 0, 0, 0, 1, '2011-11-11', 12676, 0, '2017-01-01 00:00:00', 0), \
('WFAD-1490', '', 'http://96.31.83.86:8200/', 'http://www.foxsportsvermont.com/', 'WFAD-1490.png', 0, 0, 0, 1, '2011-11-11', 12677, 0, '2017-01-01 00:00:00', 0), \
('WTAM-1100', '', 'http://96.31.83.86:8200/', 'http://www.wtam.com/', 'WTAM-1100.png', 0, 0, 0, 1, '2011-11-11', 12678, 0, '2017-01-01 00:00:00', 0), \
('The-Rapture', '', 'http://96.31.83.86:8200/', '', 'The-Rapture.png', 0, 0, 0, 1, '2011-11-11', 12679, 0, '2017-01-01 00:00:00', 0), \
('Edie-Brickell--New-Bohemians', '', 'http://96.31.83.86:8200/', '', 'Edie-Brickell--New-Bohemians.png', 0, 0, 0, 1, '2011-11-11', 12680, 0, '2017-01-01 00:00:00', 0), \
('KVNA-600', 'The only place in Flagstaff where you can hear late-breaking local news!', 'http://96.31.83.86:8200/', 'http://www.myradioplace.com/am600/am600.htm', 'KVNA-600.png', 0, 0, 0, 1, '2011-11-11', 12681, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1000', '', 'http://96.31.83.86:8200/', 'http://sports.espn.go.com/stations/espnradio1000/', 'ESPN-1000.png', 0, 0, 0, 1, '2011-11-11', 12682, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-Inspiration', '', 'http://96.31.83.86:8200/', '', 'Sunday-Morning-Inspiration.png', 0, 0, 0, 1, '2011-11-11', 12683, 0, '2017-01-01 00:00:00', 0), \
('Avto-Radio-903', '', 'http://96.31.83.86:8200/', 'http://www.avtoradio.ru/', 'Avto-Radio-903.png', 0, 0, 0, 1, '2011-11-11', 12684, 0, '2017-01-01 00:00:00', 0), \
('Rosa-renditions-Special', '', 'http://96.31.83.86:8200/', '', 'Rosa-renditions-Special.png', 0, 0, 0, 1, '2011-11-11', 12685, 0, '2017-01-01 00:00:00', 0), \
('Draino-Radio', '', 'http://96.31.83.86:8200/', 'http://drainoradio.com/', 'Draino-Radio.png', 0, 0, 0, 1, '2011-11-11', 12686, 0, '2017-01-01 00:00:00', 0), \
('Out-of-Bounds', '', 'http://96.31.83.86:8200/', '', 'Out-of-Bounds.png', 0, 0, 0, 1, '2011-11-11', 12687, 0, '2017-01-01 00:00:00', 0), \
('The-Ump-730', '', 'http://96.31.83.86:8200/', 'http://www.730ump.com/', 'The-Ump-730.png', 0, 0, 0, 1, '2011-11-11', 12688, 0, '2017-01-01 00:00:00', 0), \
('KPUS-1045', 'Corpus Christi&#39;s Classic Rock Station.', 'http://96.31.83.86:8200/', 'http://www.classicrock1045.com/', 'KPUS-1045.png', 0, 0, 0, 1, '2011-11-11', 12689, 0, '2017-01-01 00:00:00', 0), \
('937-Bob-FM', '', 'http://96.31.83.86:8200/', 'http://www.937bobfm.com/', '937-Bob-FM.png', 0, 0, 0, 1, '2011-11-11', 12690, 0, '2017-01-01 00:00:00', 0), \
('Spirit-1059', 'SPIRIT 105.9, The Spirit of Austin, features Adult Contemporary Christian music; a popular music format that appeals to a wide range of listeners.', 'http://96.31.83.86:8200/', 'http://www.spirit1059.com/', 'Spirit-1059.png', 0, 0, 0, 1, '2011-11-11', 12691, 0, '2017-01-01 00:00:00', 0), \
('Heaven-887', '', 'http://96.31.83.86:8200/', 'http://www.kfbn.org/', 'Heaven-887.png', 0, 0, 0, 1, '2011-11-11', 12692, 0, '2017-01-01 00:00:00', 0), \
('Mix-96-961', '', 'http://96.31.83.86:8200/', 'http://www.totalradio.com/96x.htm', 'Mix-96-961.png', 0, 0, 0, 1, '2011-11-11', 12693, 0, '2017-01-01 00:00:00', 0), \
('WIGV-LP-965', '', 'http://96.31.83.86:8200/', 'http://radiorenacerri.com/', 'WIGV-LP-965.png', 0, 0, 0, 1, '2011-11-11', 12694, 0, '2017-01-01 00:00:00', 0), \
('Hallelujah-1043', '', 'http://96.31.83.86:8200/', 'http://www.1043hallelujahfm.com/', 'Hallelujah-1043.png', 0, 0, 0, 1, '2011-11-11', 12695, 0, '2017-01-01 00:00:00', 0), \
('Worship-Word-Warfare', '', 'http://96.31.83.86:8200/', '', 'Worship-Word-Warfare.png', 0, 0, 0, 1, '2011-11-11', 12696, 0, '2017-01-01 00:00:00', 0), \
('Good-Time-Oldies', '', 'http://96.31.83.86:8200/', '', 'Good-Time-Oldies.png', 0, 0, 0, 1, '2011-11-11', 12697, 0, '2017-01-01 00:00:00', 0), \
('1046-FM-RTL-Best-of-Black', 'The Best of Black &quot;gives you the highlights from Hip-Hop, R &amp; B and Soul direct and unfiltered as a web exclusive!', 'http://96.31.83.86:8200/', 'http://104.6rtl.com/', '1046-FM-RTL-Best-of-Black.png', 0, 0, 0, 1, '2011-11-11', 12698, 0, '2017-01-01 00:00:00', 0), \
('Dallas-Stars-at-Columbus-Blue-Jackets-Oct-18-2011', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 12699, 0, '2017-01-01 00:00:00', 0), \
('WESC-FM-925', '', 'http://96.31.83.86:8200/', 'http://www.wescfm.com/', 'WESC-FM-925.png', 0, 0, 0, 1, '2011-11-11', 12701, 0, '2017-01-01 00:00:00', 0), \
('Peace-FM-1043', '', 'http://96.31.83.86:8200/', 'http://www.peacefmonline.com/', 'Peace-FM-1043.png', 0, 0, 0, 1, '2011-11-11', 12702, 0, '2017-01-01 00:00:00', 0), \
('Fox-NFL-Rewind', '', 'http://96.31.83.86:8200/', '', 'Fox-NFL-Rewind.png', 0, 0, 0, 1, '2011-11-11', 12703, 0, '2017-01-01 00:00:00', 0), \
('Radio-Nueva-Vida-903', '', 'http://96.31.83.86:8200/', 'http://www.nuevavida.com/', 'Radio-Nueva-Vida-903.png', 0, 0, 0, 1, '2011-11-11', 12704, 0, '2017-01-01 00:00:00', 0), \
('X1039', '', 'http://96.31.83.86:8200/', 'http://www.x1039.com/', 'X1039.png', 0, 0, 0, 1, '2011-11-11', 12706, 0, '2017-01-01 00:00:00', 0), \
('Clubbin-One-Radio', 'Dance actuelle, dance Golds 90&#39;s ! Le mix dancefloor music !', 'http://96.31.83.86:8200/', 'http://www.clubbinone.fr/', 'Clubbin-One-Radio.png', 0, 0, 0, 1, '2011-11-11', 12707, 0, '2017-01-01 00:00:00', 0), \
('KNON-893', 'KNON is a non-profit, listener-supported radio station, deriving its main source of income from on-air pledge drives and from underwriting or sponsorships by local small businesses.', 'http://96.31.83.86:8200/', 'http://www.knon.org/', 'KNON-893.png', 0, 0, 0, 1, '2011-11-11', 12708, 0, '2017-01-01 00:00:00', 0), \
('The-Beat-1017', '', 'http://96.31.83.86:8200/', 'http://www.1017thebeat.com/', 'The-Beat-1017.png', 0, 0, 0, 1, '2011-11-11', 12709, 0, '2017-01-01 00:00:00', 0), \
('WNXT-1260', 'Today much of WNXT is dominated by ESPN Radio and local sports. The station dumped much of its country music in 2004 for ESPN Radio. The station has a major variety of both local and professional spor', 'http://96.31.83.86:8200/', 'http://www.angelfire.com/oh3/wnxt/wnxtam.html', 'WNXT-1260.png', 0, 0, 0, 1, '2011-11-11', 12710, 0, '2017-01-01 00:00:00', 0), \
('The-Voice-720', '', 'http://96.31.83.86:8200/', 'http://www.newsradio720.com/', 'The-Voice-720.png', 0, 0, 0, 1, '2011-11-11', 12711, 0, '2017-01-01 00:00:00', 0), \
('DIE-NEUE-1077', '', 'http://96.31.83.86:8200/', 'http://www.dieneue1077.de/', 'DIE-NEUE-1077.png', 0, 0, 0, 1, '2011-11-11', 12712, 0, '2017-01-01 00:00:00', 0), \
('TapouT-Radio', 'The worlds #1 MMA radio show. We have the biggest names in MMA every Monday and Thursday at 9PM Eastern, and replays streaming 24/7.', 'http://96.31.83.86:8200/', 'http://www.tapoutlive.com', 'TapouT-Radio.png', 0, 0, 0, 1, '2011-11-11', 12713, 0, '2017-01-01 00:00:00', 0), \
('Z-969', '', 'http://96.31.83.86:8200/', 'http://www.kzbkradio.com/', 'Z-969.png', 0, 0, 0, 1, '2011-11-11', 12714, 0, '2017-01-01 00:00:00', 0), \
('WINZ-940', '', 'http://96.31.83.86:8200/', 'http://www.940winz.com', 'WINZ-940.png', 0, 0, 0, 1, '2011-11-11', 12715, 0, '2017-01-01 00:00:00', 0), \
('SNR-Postgame', '', 'http://96.31.83.86:8200/', '', 'SNR-Postgame.png', 0, 0, 0, 1, '2011-11-11', 12716, 0, '2017-01-01 00:00:00', 0), \
('Ill-Nio', '', 'http://96.31.83.86:8200/', '', 'Ill-Nio.png', 0, 0, 0, 1, '2011-11-11', 12717, 0, '2017-01-01 00:00:00', 0), \
('KBIG-979', 'KBIG 97.9 Hawaii and 106.1 Kona are Piped from the Big Island of Hawaii! Playing &quot;Today&#39;s Hits and Yesterday&#39;s favorites&quot;, with a sprinkle of Hawaiian Music!', 'http://96.31.83.86:8200/', 'http://www.kbigfm.com/', 'KBIG-979.png', 0, 0, 0, 1, '2011-11-11', 12718, 0, '2017-01-01 00:00:00', 0), \
('Vancouver-Canucks-Play-by-Play', '', 'http://96.31.83.86:8200/', '', 'Vancouver-Canucks-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 12719, 0, '2017-01-01 00:00:00', 0), \
('House-Nation-UK', 'HouseNationUK Radio broadcasts live House Music 24 hours a day! Tune in to hear our resident DJs from nationally established venues such as: Gatecrasher, Air, Ministry Of Sound, Stealth and The Custar', 'http://96.31.83.86:8200/', 'http://housenationuk.com/', 'House-Nation-UK.png', 0, 0, 0, 1, '2011-11-11', 12720, 0, '2017-01-01 00:00:00', 0), \
('David-Gray', '', 'http://96.31.83.86:8200/', '', 'David-Gray.png', 0, 0, 0, 1, '2011-11-11', 12721, 0, '2017-01-01 00:00:00', 0), \
('KDAA-1031', '', 'http://96.31.83.86:8200/', 'http://www.resultsradioonline.com/', 'KDAA-1031.png', 0, 0, 0, 1, '2011-11-11', 12722, 0, '2017-01-01 00:00:00', 0), \
('Linas-Jazz', '', 'http://96.31.83.86:8200/', 'http://www.linasjazz.net/', 'Linas-Jazz.png', 0, 0, 0, 1, '2011-11-11', 12723, 0, '2017-01-01 00:00:00', 0), \
('FM-100-1003', '', 'http://96.31.83.86:8200/', 'http://www.fm100.com/', 'FM-100-1003.png', 0, 0, 0, 1, '2011-11-11', 12724, 0, '2017-01-01 00:00:00', 0), \
('995-WYCD', '99.5 WYCD is Detroit&#39;s Premier Country Station with hot New Artists like Rascal Flatts, Carrie Underwood, Tim McGraw, Taylor Swift and Kenny Chesney blended with core artists like Garth Brooks, Ge', 'http://96.31.83.86:8200/', 'http://www.wycd.com/', '995-WYCD.png', 0, 0, 0, 1, '2011-11-11', 12725, 0, '2017-01-01 00:00:00', 0), \
('Inside-Alabama-Racing', '', 'http://96.31.83.86:8200/', '', 'Inside-Alabama-Racing.png', 0, 0, 0, 1, '2011-11-11', 12727, 0, '2017-01-01 00:00:00', 0), \
('Gas-a-Fondo', '', 'http://96.31.83.86:8200/', '', 'Gas-a-Fondo.png', 0, 0, 0, 1, '2011-11-11', 12728, 0, '2017-01-01 00:00:00', 0), \
('Gospel-Experience', '', 'http://96.31.83.86:8200/', '', 'Gospel-Experience.png', 0, 0, 0, 1, '2011-11-11', 12729, 0, '2017-01-01 00:00:00', 0), \
('Answering-The-Call', '', 'http://96.31.83.86:8200/', '', 'Answering-The-Call.png', 0, 0, 0, 1, '2011-11-11', 12730, 0, '2017-01-01 00:00:00', 0), \
('Generation-X', '', 'http://96.31.83.86:8200/', '', 'Generation-X.png', 0, 0, 0, 1, '2011-11-11', 12731, 0, '2017-01-01 00:00:00', 0), \
('1CLUBFM--Dance-Hits-Chicago', 'Today&#39;s dance hits live from Chicago: Visit our website for videos and more.', 'http://96.31.83.86:8200/', 'http://www.1club.fm/', '1CLUBFM--Dance-Hits-Chicago.png', 0, 0, 0, 1, '2011-11-11', 12732, 0, '2017-01-01 00:00:00', 0), \
('GotRadio-RB-Classics', 'Get down with the classic R&amp;B and soul sounds from the days of Motown and beyond.', 'http://96.31.83.86:8200/', 'http://www.gotradio.com/', 'GotRadio-RB-Classics.png', 0, 0, 0, 1, '2011-11-11', 12733, 0, '2017-01-01 00:00:00', 0), \
('Paul-Jones', '', 'http://96.31.83.86:8200/', '', 'Paul-Jones.png', 0, 0, 0, 1, '2011-11-11', 12734, 0, '2017-01-01 00:00:00', 0), \
('Rock-935', '', 'http://96.31.83.86:8200/', 'http://www.warq.com/', 'Rock-935.png', 0, 0, 0, 1, '2011-11-11', 12735, 0, '2017-01-01 00:00:00', 0), \
('Country-Legends-1039', 'Legendary country music, concerts, everything Louisville', 'http://96.31.83.86:8200/', 'http://countrylegends1039.com/', 'Country-Legends-1039.png', 0, 0, 0, 1, '2011-11-11', 12736, 0, '2017-01-01 00:00:00', 0), \
('KPIG-FM-1075', '', 'http://96.31.83.86:8200/', 'http://www.kpig.com/', 'KPIG-FM-1075.png', 0, 0, 0, 1, '2011-11-11', 12737, 0, '2017-01-01 00:00:00', 0), \
('Albert-King', '', 'http://96.31.83.86:8200/', '', 'Albert-King.png', 0, 0, 0, 1, '2011-11-11', 12738, 0, '2017-01-01 00:00:00', 0), \
('Triple-M-1051', '', 'http://96.31.83.86:8200/', 'http://www.triplem.com.au/melbourne', 'Triple-M-1051.png', 0, 0, 0, 1, '2011-11-11', 12739, 0, '2017-01-01 00:00:00', 0), \
('WIND-560', '', 'http://96.31.83.86:8200/', 'http://www.560wind.com/', 'WIND-560.png', 0, 0, 0, 1, '2011-11-11', 12740, 0, '2017-01-01 00:00:00', 0), \
('Andy-Bey', '', 'http://96.31.83.86:8200/', '', 'Andy-Bey.png', 0, 0, 0, 1, '2011-11-11', 12741, 0, '2017-01-01 00:00:00', 0), \
('Thunder-106-1063', '', 'http://96.31.83.86:8200/', 'http://www.thunder106.com/', 'Thunder-106-1063.png', 0, 0, 0, 1, '2011-11-11', 12742, 0, '2017-01-01 00:00:00', 0), \
('Faith-Memorial-Church', '', 'http://96.31.83.86:8200/', '', 'Faith-Memorial-Church.png', 0, 0, 0, 1, '2011-11-11', 12743, 0, '2017-01-01 00:00:00', 0), \
('Classic-Jenny-FM', '', 'http://96.31.83.86:8200/', 'http://classic.jenny.fm/', 'Classic-Jenny-FM.png', 0, 0, 0, 1, '2011-11-11', 12744, 0, '2017-01-01 00:00:00', 0), \
('Dallas-Stars-at-Anaheim-Ducks-Oct-21-2011', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 12745, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-Memphis-730', '', 'http://96.31.83.86:8200/', 'http://www.730foxsports.com/', 'Fox-Sports-Memphis-730.png', 0, 0, 0, 1, '2011-11-11', 12746, 0, '2017-01-01 00:00:00', 0), \
('KZIM-960', '', 'http://96.31.83.86:8200/', 'http://www.960kzim.com/', 'KZIM-960.png', 0, 0, 0, 1, '2011-11-11', 12747, 0, '2017-01-01 00:00:00', 0), \
('Salt-and-Light-Catholic-Radio-1140', '', 'http://96.31.83.86:8200/', 'http://www.saltandlightradio.com/', 'Salt-and-Light-Catholic-Radio-1140.png', 0, 0, 0, 1, '2011-11-11', 12748, 0, '2017-01-01 00:00:00', 0), \
('WCOA-1370', '', 'http://96.31.83.86:8200/', 'http://www.wcoapensacola.com/', 'WCOA-1370.png', 0, 0, 0, 1, '2011-11-11', 12749, 0, '2017-01-01 00:00:00', 0), \
('WMAY-970', '', 'http://96.31.83.86:8200/', 'http://www.wmay.com/', 'WMAY-970.png', 0, 0, 0, 1, '2011-11-11', 12750, 0, '2017-01-01 00:00:00', 0), \
('Morning-Radio', '', 'http://96.31.83.86:8200/', '', 'Morning-Radio.png', 0, 0, 0, 1, '2011-11-11', 12751, 0, '2017-01-01 00:00:00', 0), \
('Young-Jeezy', '', 'http://96.31.83.86:8200/', '', 'Young-Jeezy.png', 0, 0, 0, 1, '2011-11-11', 12752, 0, '2017-01-01 00:00:00', 0), \
('4KQ-693', '', 'http://96.31.83.86:8200/', 'http://www.4kq.com.au/', '4KQ-693.png', 0, 0, 0, 1, '2011-11-11', 12753, 0, '2017-01-01 00:00:00', 0), \
('Motley-Fool-Money-Radio-Show', '', 'http://96.31.83.86:8200/', '', 'Motley-Fool-Money-Radio-Show.png', 0, 0, 0, 1, '2011-11-11', 12754, 0, '2017-01-01 00:00:00', 0), \
('WWJ-Newsradio-950', 'WWJ Newsradio 950 is Detroit&#39;s award winning all-news radio station and is live, local and committed to providing metro Detroiters the place to turn for up-to-the-minute news, traffic, sports, bus', 'http://96.31.83.86:8200/', 'http://www.wwj.com/', 'WWJ-Newsradio-950.png', 0, 0, 0, 1, '2011-11-11', 12755, 0, '2017-01-01 00:00:00', 0), \
('Rajawali-Radio', '', 'http://96.31.83.86:8200/', 'http://rajawaliradio.com/', 'Rajawali-Radio.png', 0, 0, 0, 1, '2011-11-11', 12756, 0, '2017-01-01 00:00:00', 0), \
('Ray-LaMontagne', '', 'http://96.31.83.86:8200/', '', 'Ray-LaMontagne.png', 0, 0, 0, 1, '2011-11-11', 12757, 0, '2017-01-01 00:00:00', 0), \
('965-KLR-965', '', 'http://96.31.83.86:8200/', 'http://965klr.com/', '965-KLR-965.png', 0, 0, 0, 1, '2011-11-11', 12758, 0, '2017-01-01 00:00:00', 0), \
('101-5-Bob-Rocks-1015', '', 'http://96.31.83.86:8200/', 'http://www.1015bobrocks.com/', '101-5-Bob-Rocks-1015.png', 0, 0, 0, 1, '2011-11-11', 12759, 0, '2017-01-01 00:00:00', 0), \
('ClubZonefm', '', 'http://96.31.83.86:8200/', 'http://www.clubzone.fm/', 'ClubZonefm.png', 0, 0, 0, 1, '2011-11-11', 12760, 0, '2017-01-01 00:00:00', 0), \
('Pro-Wrestling-Report', '', 'http://96.31.83.86:8200/', '', 'Pro-Wrestling-Report.png', 0, 0, 0, 1, '2011-11-11', 12761, 0, '2017-01-01 00:00:00', 0), \
('River-Country-1075', '', 'http://96.31.83.86:8200/', 'http://www.wnntfm.com/', 'River-Country-1075.png', 0, 0, 0, 1, '2011-11-11', 12762, 0, '2017-01-01 00:00:00', 0), \
('Talkin-Baseball', '', 'http://96.31.83.86:8200/', '', 'Talkin-Baseball.png', 0, 0, 0, 1, '2011-11-11', 12763, 0, '2017-01-01 00:00:00', 0), \
('GotRadio-Indie-Underground', 'The best of the best of undiscovered, untapped and unsigned rock.  Discover what&#39;s coming out of your neighborhood garages!', 'http://96.31.83.86:8200/', 'http://www.gotradio.com/', 'GotRadio-Indie-Underground.png', 0, 0, 0, 1, '2011-11-11', 12765, 0, '2017-01-01 00:00:00', 0), \
('Ceonta', '', 'http://96.31.83.86:8200/', '', 'Ceonta.png', 0, 0, 0, 1, '2011-11-11', 12766, 0, '2017-01-01 00:00:00', 0), \
('Chillout-Ibiza-FM', 'Simply the best mix of cool relaxing chillout music to sooth away any stressful day. Mixed in superb quality FM stereo and designed to take you places where your worries drift away. Welcome to Chillou', 'http://96.31.83.86:8200/', 'http://www.chilloutibizafm.com/', 'Chillout-Ibiza-FM.png', 0, 0, 0, 1, '2011-11-11', 12767, 0, '2017-01-01 00:00:00', 0), \
('Dorothy-Ashby', '', 'http://96.31.83.86:8200/', '', 'Dorothy-Ashby.png', 0, 0, 0, 1, '2011-11-11', 12768, 0, '2017-01-01 00:00:00', 0), \
('The-Academy-Is', '', 'http://96.31.83.86:8200/', '', 'The-Academy-Is.png', 0, 0, 0, 1, '2011-11-11', 12769, 0, '2017-01-01 00:00:00', 0), \
('Big-1290', '', 'http://96.31.83.86:8200/', 'http://www.wfbg.com/', 'Big-1290.png', 0, 0, 0, 1, '2011-11-11', 12770, 0, '2017-01-01 00:00:00', 0), \
('KLBJ-FM-937', '', 'http://96.31.83.86:8200/', 'http://www.klbjfm.com/', 'KLBJ-FM-937.png', 0, 0, 0, 1, '2011-11-11', 12771, 0, '2017-01-01 00:00:00', 0), \
('WSM-FM-955', '', 'http://96.31.83.86:8200/', 'http://www.955thewolf.com/', 'WSM-FM-955.png', 0, 0, 0, 1, '2011-11-11', 12772, 0, '2017-01-01 00:00:00', 0), \
('The-Marshall-Tucker-Band', '', 'http://96.31.83.86:8200/', '', 'The-Marshall-Tucker-Band.png', 0, 0, 0, 1, '2011-11-11', 12773, 0, '2017-01-01 00:00:00', 0), \
('WWNR-620', '', 'http://96.31.83.86:8200/', 'http://www.wwnrnewstalk620.com/', 'WWNR-620.png', 0, 0, 0, 1, '2011-11-11', 12774, 0, '2017-01-01 00:00:00', 0), \
('At-the-Track', '', 'http://96.31.83.86:8200/', '', 'At-the-Track.png', 0, 0, 0, 1, '2011-11-11', 12775, 0, '2017-01-01 00:00:00', 0), \
('Underoath', '', 'http://96.31.83.86:8200/', '', 'Underoath.png', 0, 0, 0, 1, '2011-11-11', 12776, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Afternoon-Jazz-(CJRT)', '', 'http://96.31.83.86:8200/', '', 'Sunday-Afternoon-Jazz-(CJRT).png', 0, 0, 0, 1, '2011-11-11', 12777, 0, '2017-01-01 00:00:00', 0), \
('CountryManiaRadio', 'CountryManiaRadio is a radio who&#39;s listening to there listerners. At all the time the listerners can make request even in the non-stop, see for more information the website.<BR>Each Dj has is/here', 'http://96.31.83.86:8200/', 'http://www.countrymaniaradio.com/', 'CountryManiaRadio.png', 0, 0, 0, 1, '2011-11-11', 12779, 0, '2017-01-01 00:00:00', 0), \
('WGSO-990', 'WGSO 990 is a news, talk, and sports AM radio station based in New Orleans, Louisiana.', 'http://96.31.83.86:8200/', 'http://wgso.com/', 'WGSO-990.png', 0, 0, 0, 1, '2011-11-11', 12780, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1500', '', 'http://96.31.83.86:8200/', 'http://www.waysam.com/', 'The-Fan-1500.png', 0, 0, 0, 1, '2011-11-11', 12781, 0, '2017-01-01 00:00:00', 0), \
('Jelly-Radio', 'JellyRadio.com, an Internet music source for hip-hop and RB', 'http://96.31.83.86:8200/', 'http://www.jellyradio.com/', 'Jelly-Radio.png', 0, 0, 0, 1, '2011-11-11', 12782, 0, '2017-01-01 00:00:00', 0), \
('Radio-Bop', 'Radio Bop plays 50s and 60s Rock &#39;n Roll from early Rockabilly through early Beatles with a playlist based on the Billboard Top 40 from mid-1955 through 1965.', 'http://96.31.83.86:8200/', 'http://www.radiobop.com/', 'Radio-Bop.png', 0, 0, 0, 1, '2011-11-11', 12783, 0, '2017-01-01 00:00:00', 0), \
('Defjay-Radio', '100% R&amp;B Radio! The place to be in R&amp;B!', 'http://96.31.83.86:8200/', 'http://www.defjay.com/', 'Defjay-Radio.png', 0, 0, 0, 1, '2011-11-11', 12784, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Heavy-Hitting-Boxing-Podcast', '', 'http://96.31.83.86:8200/', '', 'ESPN-Heavy-Hitting-Boxing-Podcast.png', 0, 0, 0, 1, '2011-11-11', 12785, 0, '2017-01-01 00:00:00', 0), \
('WLRN-FM-913', '', 'http://96.31.83.86:8200/', 'http://www.wlrn.org/', 'WLRN-FM-913.png', 0, 0, 0, 1, '2011-11-11', 12786, 0, '2017-01-01 00:00:00', 0), \
('WLQY-1320', '', 'http://96.31.83.86:8200/', 'http://www.doucefm.com', 'WLQY-1320.png', 0, 0, 0, 1, '2011-11-11', 12787, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1330', '', 'http://96.31.83.86:8200/', 'http://www.sportsradio1330.com/', 'The-Fan-1330.png', 0, 0, 0, 1, '2011-11-11', 12788, 0, '2017-01-01 00:00:00', 0), \
('WCMF-FM-965', '', 'http://96.31.83.86:8200/', 'http://www.wcmf.com/', 'WCMF-FM-965.png', 0, 0, 0, 1, '2011-11-11', 12789, 0, '2017-01-01 00:00:00', 0), \
('WRMT-1490', '', 'http://96.31.83.86:8200/', '', 'WRMT-1490.png', 0, 0, 0, 1, '2011-11-11', 12790, 0, '2017-01-01 00:00:00', 0), \
('House-Smarts', '', 'http://96.31.83.86:8200/', '', 'House-Smarts.png', 0, 0, 0, 1, '2011-11-11', 12791, 0, '2017-01-01 00:00:00', 0), \
('John-Fogerty', '', 'http://96.31.83.86:8200/', '', 'John-Fogerty.png', 0, 0, 0, 1, '2011-11-11', 12792, 0, '2017-01-01 00:00:00', 0), \
('WRXB-1590', '', 'http://96.31.83.86:8200/', 'http://www.wrxb.us/', 'WRXB-1590.png', 0, 0, 0, 1, '2011-11-11', 12793, 0, '2017-01-01 00:00:00', 0), \
('Eurodance-90-Radio', '', 'http://96.31.83.86:8200/', 'http://eurodance90.fr/', 'Eurodance-90-Radio.png', 0, 0, 0, 1, '2011-11-11', 12794, 0, '2017-01-01 00:00:00', 0), \
('Lite-Rock-973', '', 'http://96.31.83.86:8200/', 'http://www.literock973.com/', 'Lite-Rock-973.png', 0, 0, 0, 1, '2011-11-11', 12795, 0, '2017-01-01 00:00:00', 0), \
('Mix-929', '', 'http://96.31.83.86:8200/', 'http://www.mymix929.com/', 'Mix-929.png', 0, 0, 0, 1, '2011-11-11', 12796, 0, '2017-01-01 00:00:00', 0), \
('NCB-Radio', 'NCB Radio was founded in March 2010 by a group of young people in North Cornwall with one single aim- to create a radio station that is more than just a radio station. It is our mission to use the med', 'http://96.31.83.86:8200/', 'http://www.ncbradio.co.uk/', 'NCB-Radio.png', 0, 0, 0, 1, '2011-11-11', 12797, 0, '2017-01-01 00:00:00', 0), \
('Alanis-Morissette', '', 'http://96.31.83.86:8200/', '', 'Alanis-Morissette.png', 0, 0, 1, 1, '2011-11-11', 12798, 0, '2011-10-17 23:41:29', 0), \
('After-7', '', 'http://96.31.83.86:8200/', '', 'After-7.png', 0, 0, 0, 1, '2011-11-11', 12799, 0, '2017-01-01 00:00:00', 0), \
('Ra', '', 'http://96.31.83.86:8200/', '', 'Ra.png', 0, 0, 0, 1, '2011-11-11', 12800, 0, '2017-01-01 00:00:00', 0), \
('3-Doors-Down', '', 'http://96.31.83.86:8200/', '', '3-Doors-Down.png', 0, 0, 0, 1, '2011-11-11', 12801, 0, '2017-01-01 00:00:00', 0), \
('WHGL-FM-1003', '', 'http://96.31.83.86:8200/', 'http://www.wiggle100.com/', 'WHGL-FM-1003.png', 0, 0, 0, 1, '2011-11-11', 12802, 0, '2017-01-01 00:00:00', 0), \
('KHUB-1340', '', 'http://96.31.83.86:8200/', 'http://www.khubradio.com/', 'KHUB-1340.png', 0, 0, 0, 1, '2011-11-11', 12803, 0, '2017-01-01 00:00:00', 0), \
('4KIG-1071', '', 'http://96.31.83.86:8200/', 'http://www.4k1g.org/', '4KIG-1071.png', 0, 0, 0, 1, '2011-11-11', 12804, 0, '2017-01-01 00:00:00', 0), \
('Beatles-Radio', '', 'http://96.31.83.86:8200/', 'http://www.beatleradio.com/', 'Beatles-Radio.png', 0, 0, 0, 1, '2011-11-11', 12805, 0, '2017-01-01 00:00:00', 0), \
('Sunny-1043', 'Sunny 104.3 has been South Floridas number one choice for music variety for over 15 years!', 'http://96.31.83.86:8200/', 'http://www.sunny1043.com/', 'Sunny-1043.png', 0, 0, 0, 1, '2011-11-11', 12807, 0, '2017-01-01 00:00:00', 0), \
('Steppin-Out', '', 'http://96.31.83.86:8200/', '', 'Steppin-Out.png', 0, 0, 0, 1, '2011-11-11', 12808, 0, '2017-01-01 00:00:00', 0), \
('Your-Money-Matters', 'Your Money Matters! promises to deliver useful information in an entertaining format that will help you in your everyday financial lives.<BR><BR>Each week your host Marc Pearlman will interview leadin', 'http://96.31.83.86:8200/', 'http://www.yourmoneymattersradio.com/', 'Your-Money-Matters.png', 0, 0, 0, 1, '2011-11-11', 12809, 0, '2017-01-01 00:00:00', 0), \
('KGLO-1300', '', 'http://96.31.83.86:8200/', 'http://www.kgloam.com/', 'KGLO-1300.png', 0, 0, 0, 1, '2011-11-11', 12810, 0, '2017-01-01 00:00:00', 0), \
('KDQN-1390', '', 'http://96.31.83.86:8200/', 'http://www.kdqn.net/', 'KDQN-1390.png', 0, 0, 0, 1, '2011-11-11', 12811, 0, '2017-01-01 00:00:00', 0), \
('Cassius', '', 'http://96.31.83.86:8200/', '', 'Cassius.png', 0, 0, 0, 1, '2011-11-11', 12812, 0, '2017-01-01 00:00:00', 0), \
('The-Mustard--Johnson-Show', '', 'http://96.31.83.86:8200/', '', 'The-Mustard--Johnson-Show.png', 0, 0, 0, 1, '2011-11-11', 12813, 0, '2017-01-01 00:00:00', 0), \
('Puma-Radio', '', 'http://96.31.83.86:8200/', 'http://www.radio-puma.de/', 'Puma-Radio.png', 0, 0, 0, 1, '2011-11-11', 12814, 0, '2017-01-01 00:00:00', 0), \
('Thelma-Houston', '', 'http://96.31.83.86:8200/', '', 'Thelma-Houston.png', 0, 0, 0, 1, '2011-11-11', 12816, 0, '2017-01-01 00:00:00', 0), \
('Joe-Jackson', '', 'http://96.31.83.86:8200/', '', 'Joe-Jackson.png', 0, 0, 0, 1, '2011-11-11', 12817, 0, '2017-01-01 00:00:00', 0), \
('Swing--Big-Band-on-JAZZRADIOcom', 'The best sounds of the swing era of the 30s &amp; 40s.', 'http://96.31.83.86:8200/', 'http://www.jazzradio.com/', 'Swing--Big-Band-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 12818, 0, '2017-01-01 00:00:00', 0), \
('Deutschlandradio-Kultur-896', 'Deutschlandradio Kultur ist umfassender H&#246;rgenuss - f&#252;r Menschen, denen Musik, Kulturinformationen und anregende Unterhaltung wichtig sind.', 'http://96.31.83.86:8200/', 'http://www.dradio.de/dkultur/', 'Deutschlandradio-Kultur-896.png', 0, 0, 0, 1, '2011-11-11', 12819, 0, '2017-01-01 00:00:00', 0), \
('SKYFM-Alt-Rock', 'Alternative rock hits You want to hear!!', 'http://96.31.83.86:8200/', 'http://www.sky.fm/altrock', 'SKYFM-Alt-Rock.png', 0, 0, 0, 1, '2011-11-11', 12820, 0, '2017-01-01 00:00:00', 0), \
('WLJA-1011', '', 'http://96.31.83.86:8200/', 'http://www.wljaradio.com/', 'WLJA-1011.png', 0, 0, 0, 1, '2011-11-11', 12821, 0, '2017-01-01 00:00:00', 0), \
('DR P1', '', 'http://live-icy.gss.dr.dk:8000/A/A03H.mp3', 'http://www.dr.dk/p1/', 'DR-P1-908.png', 0, 0, 0, 1, '2011-11-11', 12822, 0, '2017-01-01 00:00:00', 0), \
('Fidelity-957', '', 'http://96.31.83.86:8200/', 'http://www.fidelitypr.com/', 'Fidelity-957.png', 0, 0, 0, 1, '2011-11-11', 12823, 0, '2017-01-01 00:00:00', 0), \
('ESPN-690', '', 'http://96.31.83.86:8200/', '', 'ESPN-690.png', 0, 0, 0, 1, '2011-11-11', 12824, 0, '2017-01-01 00:00:00', 0), \
('The-Cardigans', '', 'http://96.31.83.86:8200/', '', 'The-Cardigans.png', 0, 0, 0, 1, '2011-11-11', 12825, 0, '2017-01-01 00:00:00', 0), \
('KISS-Country-999', '', 'http://96.31.83.86:8200/', 'http://www.wkis.com/', 'KISS-Country-999.png', 0, 0, 0, 1, '2011-11-11', 12826, 0, '2017-01-01 00:00:00', 0), \
('Sixties-Sunday-With-Dean-Martin', '', 'http://96.31.83.86:8200/', '', 'Sixties-Sunday-With-Dean-Martin.png', 0, 0, 0, 1, '2011-11-11', 12827, 0, '2017-01-01 00:00:00', 0), \
('WPLN-1430', 'NPR Talk and World News 24 Hours a Day', 'http://96.31.83.86:8200/', 'http://wpln.org/', 'WPLN-1430.png', 0, 0, 0, 1, '2011-11-11', 12828, 0, '2017-01-01 00:00:00', 0), \
('K-Shore-973', '', 'http://96.31.83.86:8200/', 'http://www.kshr.com/', 'K-Shore-973.png', 0, 0, 0, 1, '2011-11-11', 12829, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-995', '', 'http://96.31.83.86:8200/', 'http://www.thewolfonline.com/', 'The-Wolf-995.png', 0, 0, 0, 1, '2011-11-11', 12830, 0, '2017-01-01 00:00:00', 0), \
('Cruisin-921', '', 'http://96.31.83.86:8200/', 'http://www.wvlt.com/', 'Cruisin-921.png', 0, 0, 0, 1, '2011-11-11', 12831, 0, '2017-01-01 00:00:00', 0), \
('Passport-Approved', '', 'http://96.31.83.86:8200/', '', 'Passport-Approved.png', 0, 0, 0, 1, '2011-11-11', 12832, 0, '2017-01-01 00:00:00', 0), \
('Natasha-Bedingfield', '', 'http://96.31.83.86:8200/', '', 'Natasha-Bedingfield.png', 0, 0, 0, 1, '2011-11-11', 12833, 0, '2017-01-01 00:00:00', 0), \
('WSEI-929', '', 'http://96.31.83.86:8200/', 'http://www.929thelegend.com/', 'WSEI-929.png', 0, 0, 0, 1, '2011-11-11', 12834, 0, '2017-01-01 00:00:00', 0), \
('Dead-Air', '', 'http://96.31.83.86:8200/', '', 'Dead-Air.png', 0, 0, 0, 1, '2011-11-11', 12835, 0, '2017-01-01 00:00:00', 0), \
('KMAJ-1440', '', 'http://96.31.83.86:8200/', 'http://kmaj1440.com/', 'KMAJ-1440.png', 0, 0, 0, 1, '2011-11-11', 12836, 0, '2017-01-01 00:00:00', 0), \
('Juanes', '', 'http://96.31.83.86:8200/', '', 'Juanes.png', 0, 0, 0, 1, '2011-11-11', 12837, 0, '2017-01-01 00:00:00', 0), \
('Boystown-Live-Dance-Radio', '', 'http://96.31.83.86:8200/', 'http://boystownlive.com/', 'Boystown-Live-Dance-Radio.png', 0, 0, 0, 1, '2011-11-11', 12838, 0, '2017-01-01 00:00:00', 0), \
('Infernal', '', 'http://96.31.83.86:8200/', '', 'Infernal.png', 0, 0, 0, 1, '2011-11-11', 12839, 0, '2017-01-01 00:00:00', 0), \
('KFOG-1045', '', 'http://96.31.83.86:8200/', 'http://www.kfog.com/', 'KFOG-1045.png', 0, 0, 0, 1, '2011-11-11', 12840, 0, '2017-01-01 00:00:00', 0), \
('Nashville-Classics', '', 'http://96.31.83.86:8200/', 'http://www.nashvilleclassics.com/', 'Nashville-Classics.png', 0, 0, 0, 1, '2011-11-11', 12841, 0, '2017-01-01 00:00:00', 0), \
('Froggy-949', '', 'http://96.31.83.86:8200/', 'http://www.froggyland.com/index.php?site=949_940', 'Froggy-949.png', 0, 0, 0, 1, '2011-11-11', 12843, 0, '2017-01-01 00:00:00', 0), \
('Georgia-Football-Pre-Game', '', 'http://96.31.83.86:8200/', '', 'Georgia-Football-Pre-Game.png', 0, 0, 0, 1, '2011-11-11', 12844, 0, '2017-01-01 00:00:00', 0), \
('Country-Legends-943', '', 'http://96.31.83.86:8200/', 'http://www.countrylegends943.com/', 'Country-Legends-943.png', 0, 0, 0, 1, '2011-11-11', 12845, 0, '2017-01-01 00:00:00', 0), \
('Italian-Graffiati', '', 'http://96.31.83.86:8200/', 'http://www.italiangraffiati.com/', 'Italian-Graffiati.png', 0, 0, 0, 1, '2011-11-11', 12846, 0, '2017-01-01 00:00:00', 0), \
('KNOW-FM-911', '', 'http://96.31.83.86:8200/', 'http://minnesota.publicradio.org/features/', 'KNOW-FM-911.png', 0, 0, 0, 1, '2011-11-11', 12847, 0, '2017-01-01 00:00:00', 0), \
('The-Home-Team', '', 'http://96.31.83.86:8200/', '', 'The-Home-Team.png', 0, 0, 0, 1, '2011-11-11', 12848, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1050', '', 'http://96.31.83.86:8200/', 'http://www.espndecatur.com/', 'The-Fan-1050.png', 0, 0, 0, 1, '2011-11-11', 12849, 0, '2017-01-01 00:00:00', 0), \
('Phillies-247-981', '', 'http://96.31.83.86:8200/', 'http://www.wogl.com/', 'Phillies-247-981.png', 0, 0, 0, 1, '2011-11-11', 12850, 0, '2017-01-01 00:00:00', 0), \
('Authentic-Sht', '', 'http://96.31.83.86:8200/', '', 'Authentic-Sht.png', 0, 0, 0, 1, '2011-11-11', 12851, 0, '2017-01-01 00:00:00', 0), \
('Free', '', 'http://96.31.83.86:8200/', '', 'Free.png', 0, 0, 0, 1, '2011-11-11', 12852, 0, '2017-01-01 00:00:00', 0), \
('CNN-Tech-Check-Podcast', '', 'http://96.31.83.86:8200/', '', 'CNN-Tech-Check-Podcast.png', 0, 0, 0, 1, '2011-11-11', 12853, 0, '2017-01-01 00:00:00', 0), \
('91X-911', '', 'http://96.31.83.86:8200/', 'http://www.91x.com/', '91X-911.png', 0, 0, 0, 1, '2011-11-11', 12854, 0, '2017-01-01 00:00:00', 0), \
('Grassy-Hill-Radio', '', 'http://96.31.83.86:8200/', '', 'Grassy-Hill-Radio.png', 0, 0, 0, 1, '2011-11-11', 12855, 0, '2017-01-01 00:00:00', 0), \
('KXRO-1320', 'With over 77 years of community service, we are Grays Harbor Radio 1320 KXRO AM Aberdeen WA', 'http://96.31.83.86:8200/', 'http://www.kxro.com/', 'KXRO-1320.png', 0, 0, 0, 1, '2011-11-11', 12856, 0, '2017-01-01 00:00:00', 0), \
('The-ACE--TJ-Show', '', 'http://96.31.83.86:8200/', '', 'The-ACE--TJ-Show.png', 0, 0, 0, 1, '2011-11-11', 12857, 0, '2017-01-01 00:00:00', 0), \
('La-Quinta-Estacin', '', 'http://96.31.83.86:8200/', '', 'La-Quinta-Estacin.png', 0, 0, 0, 1, '2011-11-11', 12858, 0, '2017-01-01 00:00:00', 0), \
('KVSL-1450', '', 'http://96.31.83.86:8200/', 'http://www.whitemountainradio.com/', 'KVSL-1450.png', 0, 0, 0, 1, '2011-11-11', 12859, 0, '2017-01-01 00:00:00', 0), \
('The-Outfield', '', 'http://96.31.83.86:8200/', '', 'The-Outfield.png', 0, 0, 0, 1, '2011-11-11', 12860, 0, '2017-01-01 00:00:00', 0), \
('Moist', '', 'http://96.31.83.86:8200/', '', 'Moist.png', 0, 0, 0, 1, '2011-11-11', 12861, 0, '2017-01-01 00:00:00', 0), \
('Afternoon-Jazz-with-Wendy-Fopeano', '', 'http://96.31.83.86:8200/', '', 'Afternoon-Jazz-with-Wendy-Fopeano.png', 0, 0, 0, 1, '2011-11-11', 12862, 0, '2017-01-01 00:00:00', 0), \
('1LIVE---Das-junge-Radio-des-WDR-1024', '1LIVE spielt und macht Hits. Mit aktuellen Nachrichten, Informationen sowie unterhaltenden Beitr&#228;gen 24 Stunden am Tag.', 'http://96.31.83.86:8200/', 'http://www.einslive.de/', '1LIVE---Das-junge-Radio-des-WDR-1024.png', 0, 0, 0, 1, '2011-11-11', 12863, 0, '2017-01-01 00:00:00', 0), \
('French-Affair', '', 'http://96.31.83.86:8200/', '', 'French-Affair.png', 0, 0, 0, 1, '2011-11-11', 12864, 0, '2017-01-01 00:00:00', 0), \
('Talking-Heads', '', 'http://96.31.83.86:8200/', '', 'Talking-Heads.png', 0, 0, 0, 1, '2011-11-11', 12865, 0, '2017-01-01 00:00:00', 0), \
('The-River-975', '', 'http://96.31.83.86:8200/', 'http://www.ckrv.com/', 'The-River-975.png', 0, 0, 0, 1, '2011-11-11', 12866, 0, '2017-01-01 00:00:00', 0), \
('Paradise-Lost', '', 'http://96.31.83.86:8200/', '', 'Paradise-Lost.png', 0, 0, 0, 1, '2011-11-11', 12867, 0, '2017-01-01 00:00:00', 0), \
('Swedish-House-Mafia', '', 'http://96.31.83.86:8200/', '', 'Swedish-House-Mafia.png', 0, 0, 0, 1, '2011-11-11', 12868, 0, '2017-01-01 00:00:00', 0), \
('Mix-1011', '', 'http://96.31.83.86:8200/', 'http://www.mix1011.com.au/', 'Mix-1011.png', 0, 0, 0, 1, '2011-11-11', 12869, 0, '2017-01-01 00:00:00', 0), \
('Star-1033', '', 'http://96.31.83.86:8200/', 'http://star103fm.com/', 'Star-1033.png', 0, 0, 0, 1, '2011-11-11', 12870, 0, '2017-01-01 00:00:00', 0), \
('Sunset-Rubdown', '', 'http://96.31.83.86:8200/', '', 'Sunset-Rubdown.png', 0, 0, 0, 1, '2011-11-11', 12871, 0, '2017-01-01 00:00:00', 0), \
('my-999', '', 'http://96.31.83.86:8200/', 'http://www.my999radio.com/', 'my-999.png', 0, 0, 0, 1, '2011-11-11', 12872, 0, '2017-01-01 00:00:00', 0), \
('Above-Top-Secret-Radio', '', 'http://96.31.83.86:8200/', 'http://www.abovetopsecret.com/', 'Above-Top-Secret-Radio.png', 0, 0, 0, 1, '2011-11-11', 12873, 0, '2017-01-01 00:00:00', 0), \
('The-Band', '', 'http://96.31.83.86:8200/', '', 'The-Band.png', 0, 0, 0, 1, '2011-11-11', 12874, 0, '2017-01-01 00:00:00', 0), \
('Rock-979-NRQ', '', 'http://96.31.83.86:8200/', 'http://www.nrq.com/', 'Rock-979-NRQ.png', 0, 0, 0, 1, '2011-11-11', 12875, 0, '2017-01-01 00:00:00', 0), \
('FM-Uruguay-879', 'Radio Uruguay, A Partir Del Dia 24 De Octubre De 2006 Deja La Frecuencia 90.9 Mhz Despues De 14 A&#241;os.para Cumplir Con La Resolucion Numero 1755/06 Del Comfer ,por La Cual Fue Asignada La Misma Pr', 'http://96.31.83.86:8200/', 'http://www.fmuruguay.com.ar/', 'FM-Uruguay-879.png', 0, 0, 0, 1, '2011-11-11', 12876, 0, '2017-01-01 00:00:00', 0), \
('SomaFM-Secret-Agent', 'Only from SomaFM.com.', 'http://96.31.83.86:8200/', 'http://somafm.com/play/secretagent', 'SomaFM-Secret-Agent.png', 0, 0, 0, 1, '2011-11-11', 12877, 0, '2017-01-01 00:00:00', 0), \
('WLLM-1370', 'WLLM AM1370 Easy Listening Christian Radio in Lincoln, IL and Central Illinois.  Looking for Christian Radio Stations in Central Illinois', 'http://96.31.83.86:8200/', 'http://www.wllmradio.com/', 'WLLM-1370.png', 0, 0, 0, 1, '2011-11-11', 12878, 0, '2017-01-01 00:00:00', 0), \
('Health-Talk', '', 'http://96.31.83.86:8200/', '', 'Health-Talk.png', 0, 0, 0, 1, '2011-11-11', 12879, 0, '2017-01-01 00:00:00', 0), \
('Medina', '', 'http://96.31.83.86:8200/', '', 'Medina.png', 0, 0, 0, 1, '2011-11-11', 12880, 0, '2017-01-01 00:00:00', 0), \
('Kiss-1053', '', 'http://96.31.83.86:8200/', 'http://www.kiss1053.com/', 'Kiss-1053.png', 0, 0, 0, 1, '2011-11-11', 12881, 0, '2017-01-01 00:00:00', 0), \
('Exa-FM-975', '', 'http://96.31.83.86:8200/', 'http://www.exafm.com.mx/guatemala', 'Exa-FM-975.png', 0, 0, 0, 1, '2011-11-11', 12882, 0, '2017-01-01 00:00:00', 0), \
('Dig-This', '', 'http://96.31.83.86:8200/', '', 'Dig-This.png', 0, 0, 0, 1, '2011-11-11', 12883, 0, '2017-01-01 00:00:00', 0), \
('NHL-Play-by-Play', 'The puck drops every day on the world&#39;s fastest sport on the world&#39;s first hockey channel.', 'http://96.31.83.86:8200/', 'http://www.xmradio.com/', 'NHL-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 12884, 0, '2017-01-01 00:00:00', 0), \
('Post-Harbor', '', 'http://96.31.83.86:8200/', '', 'Post-Harbor.png', 0, 0, 0, 1, '2011-11-11', 12885, 0, '2017-01-01 00:00:00', 0), \
('Radio-Polskie---Soul', '', 'http://96.31.83.86:8200/', 'http://www.polskastacja.pl/', 'Radio-Polskie---Soul.png', 0, 0, 0, 1, '2011-11-11', 12886, 0, '2017-01-01 00:00:00', 0), \
('KINN-1270', '', 'http://96.31.83.86:8200/', '', 'KINN-1270.png', 0, 0, 0, 1, '2011-11-11', 12887, 0, '2017-01-01 00:00:00', 0), \
('The-Great-Outdoors', '', 'http://96.31.83.86:8200/', '', 'The-Great-Outdoors.png', 0, 0, 0, 1, '2011-11-11', 12888, 0, '2017-01-01 00:00:00', 0), \
('Nihal', '', 'http://96.31.83.86:8200/', '', 'Nihal.png', 0, 0, 0, 1, '2011-11-11', 12889, 0, '2017-01-01 00:00:00', 0), \
('100hitz---New-Country', '100hitz.com is an online radio network of free internet radio stations, featuring multiple genres of free internet radio stations.', 'http://96.31.83.86:8200/', 'http://www.100hitz.com/', '100hitz---New-Country.png', 0, 0, 0, 1, '2011-11-11', 12890, 0, '2017-01-01 00:00:00', 0), \
('Mas-FM-941', '', 'http://96.31.83.86:8200/', 'http://www.cincoradio.com.mx/emisoras.asp?emi=XHJE', 'Mas-FM-941.png', 0, 0, 0, 1, '2011-11-11', 12891, 0, '2017-01-01 00:00:00', 0), \
('Book-Reading', '', 'http://96.31.83.86:8200/', '', 'Book-Reading.png', 0, 0, 0, 1, '2011-11-11', 12892, 0, '2017-01-01 00:00:00', 0), \
('WEBY-1330', '', 'http://96.31.83.86:8200/', 'http://www.1330weby.com/', 'WEBY-1330.png', 0, 0, 0, 1, '2011-11-11', 12893, 0, '2017-01-01 00:00:00', 0), \
('Streekradio-1048', '', 'http://96.31.83.86:8200/', 'http://www.streekradio.com/', 'Streekradio-1048.png', 0, 0, 0, 1, '2011-11-11', 12894, 0, '2017-01-01 00:00:00', 0), \
('Z-Rock-103-1033', '', 'http://96.31.83.86:8200/', 'http://www.zrock103.com/', 'Z-Rock-103-1033.png', 0, 0, 0, 1, '2011-11-11', 12895, 0, '2017-01-01 00:00:00', 0);";


const char *radio_station_setupsql40="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Healthline', '', 'http://96.31.83.86:8200/', '', 'Healthline.png', 0, 0, 0, 1, '2011-11-11', 12897, 0, '2017-01-01 00:00:00', 0), \
('Reito-de-Viver', '', 'http://96.31.83.86:8200/', 'http://www.cliquecontraocancer.com.br/direitodeviver/', 'Radio-Direito-de-Viver.png', 0, 0, 0, 1, '2011-11-11', 12898, 0, '2017-01-01 00:00:00', 0), \
('80s-And-More', '80s Hits from the UK, USA, Aus &amp; Europe 24 hours a day', 'http://96.31.83.86:8200/', 'http://www.80sandmore.org.uk/', '80s-And-More.png', 0, 0, 0, 1, '2011-11-11', 12899, 0, '2017-01-01 00:00:00', 0), \
('KIKS-FM-1015', 'ola radio broadcasting began on July 25, 1961 when 1370 AM KALN (KIOL&#39;s former call letters) signed on the air. 1370 AM was a daytime-only operation', 'http://96.31.83.86:8200/', 'http://www.iolaradio.com/', 'KIKS-FM-1015.png', 0, 0, 0, 1, '2011-11-11', 12900, 0, '2017-01-01 00:00:00', 0), \
('Jill-Taylor', '', 'http://96.31.83.86:8200/', '', 'Jill-Taylor.png', 0, 0, 0, 1, '2011-11-11', 12901, 0, '2017-01-01 00:00:00', 0), \
('KNZZ-1100', '', 'http://96.31.83.86:8200/', 'http://www.1100knzz.com/', 'KNZZ-1100.png', 0, 0, 0, 1, '2011-11-11', 12902, 0, '2017-01-01 00:00:00', 0), \
('Eternal', '', 'http://96.31.83.86:8200/', '', 'Eternal.png', 0, 0, 0, 1, '2011-11-11', 12903, 0, '2017-01-01 00:00:00', 0), \
('Prep-Sports-Report', '', 'http://96.31.83.86:8200/', '', 'Prep-Sports-Report.png', 0, 0, 0, 1, '2011-11-11', 12904, 0, '2017-01-01 00:00:00', 0), \
('Into-the-Night-with-Tony-Bruno', '', 'http://96.31.83.86:8200/', '', 'Into-the-Night-with-Tony-Bruno.png', 0, 0, 0, 1, '2011-11-11', 12905, 0, '2017-01-01 00:00:00', 0), \
('MOJO-1029', '', 'http://96.31.83.86:8200/', 'http://www.mojo1029.com', 'MOJO-1029.png', 0, 0, 0, 1, '2011-11-11', 12906, 0, '2017-01-01 00:00:00', 0), \
('Sky-Racing-Radios-Inside-Running', '', 'http://96.31.83.86:8200/', '', 'Sky-Racing-Radios-Inside-Running.png', 0, 0, 0, 1, '2011-11-11', 12907, 0, '2017-01-01 00:00:00', 0), \
('973-The-Hawk', '', 'http://96.31.83.86:8200/', 'http://www.973thehawk.com/', '973-The-Hawk.png', 0, 0, 0, 1, '2011-11-11', 12908, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Evening-Jazz-with-Erik-Troe', '', 'http://96.31.83.86:8200/', '', 'Sunday-Evening-Jazz-with-Erik-Troe.png', 0, 0, 0, 1, '2011-11-11', 12909, 0, '2017-01-01 00:00:00', 0), \
('Marc-Anthony', '', 'http://96.31.83.86:8200/', '', 'Marc-Anthony.png', 0, 0, 0, 1, '2011-11-11', 12911, 0, '2017-01-01 00:00:00', 0), \
('MUSIC-AND-SERVICE-ANNOUNCEMENTS', '', 'http://96.31.83.86:8200/', '', 'MUSIC-AND-SERVICE-ANNOUNCEMENTS.png', 0, 0, 0, 1, '2011-11-11', 12912, 0, '2017-01-01 00:00:00', 0), \
('Bridges-with-Africa', '', 'http://96.31.83.86:8200/', '', 'Bridges-with-Africa.png', 0, 0, 0, 1, '2011-11-11', 12913, 0, '2017-01-01 00:00:00', 0), \
('Mix-929', '', 'http://96.31.83.86:8200/', 'http://www.mix929.com/', 'Mix-929.png', 0, 0, 0, 1, '2011-11-11', 12914, 0, '2017-01-01 00:00:00', 0), \
('Justin-Timberlake', '', 'http://96.31.83.86:8200/', '', 'Justin-Timberlake.png', 0, 0, 0, 1, '2011-11-11', 12915, 0, '2017-01-01 00:00:00', 0), \
('WAAV-980', 'The Heritage Talk Radio station of Wilmington, North Carolina, with the leaders in today&#39;s Conservative Talk!', 'http://96.31.83.86:8200/', 'http://www.980waav.com/', 'WAAV-980.png', 0, 0, 0, 1, '2011-11-11', 12916, 0, '2017-01-01 00:00:00', 0), \
('Health-Show', '', 'http://96.31.83.86:8200/', '', 'Health-Show.png', 0, 0, 0, 1, '2011-11-11', 12917, 0, '2017-01-01 00:00:00', 0), \
('Kiss-Canaries-994', '', 'http://96.31.83.86:8200/', 'http://www.kisscanaries.com', 'Kiss-Canaries-994.png', 0, 0, 0, 1, '2011-11-11', 12918, 0, '2017-01-01 00:00:00', 0), \
('The-Big-Sports-Breakfast', '', 'http://96.31.83.86:8200/', '', 'The-Big-Sports-Breakfast.png', 0, 0, 0, 1, '2011-11-11', 12919, 0, '2017-01-01 00:00:00', 0), \
('Moody-Radio-Network', 'Christian teaching and talk network feeding programming to hundreds of affiliates nationwide from studios at Moody Bible Institute in Chicago.', 'http://96.31.83.86:8200/', 'http://www.moodyradio.org/', 'Moody-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 12920, 0, '2017-01-01 00:00:00', 0), \
('Derailment-Radio', '', 'http://96.31.83.86:8200/', '', 'Derailment-Radio.png', 0, 0, 0, 1, '2011-11-11', 12921, 0, '2017-01-01 00:00:00', 0), \
('Beherit', '', 'http://96.31.83.86:8200/', '', 'Beherit.png', 0, 0, 0, 1, '2011-11-11', 12922, 0, '2017-01-01 00:00:00', 0), \
('The-Average-Joe-Show', '', 'http://96.31.83.86:8200/', '', 'The-Average-Joe-Show.png', 0, 0, 0, 1, '2011-11-11', 12923, 0, '2017-01-01 00:00:00', 0), \
('The-Pulse-1400', '', 'http://96.31.83.86:8200/', 'http://www.wtsl.com/', 'The-Pulse-1400.png', 0, 0, 0, 1, '2011-11-11', 12924, 0, '2017-01-01 00:00:00', 0), \
('Gen-X-Radio-1067', '', 'http://96.31.83.86:8200/', 'http://www.genxcolumbus.com/', 'Gen-X-Radio-1067.png', 0, 0, 0, 1, '2011-11-11', 12925, 0, '2017-01-01 00:00:00', 0), \
('Sinatra-Style-on-JAZZRADIOcom', 'For fans of Frank Sinatra, this channel is full of great American standards.', 'http://96.31.83.86:8200/', 'http://www.jazzradio.com/', 'Sinatra-Style-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 12926, 0, '2017-01-01 00:00:00', 0), \
('Witnesses-of-Faith', '', 'http://96.31.83.86:8200/', '', 'Witnesses-of-Faith.png', 0, 0, 0, 1, '2011-11-11', 12927, 0, '2017-01-01 00:00:00', 0), \
('War', '', 'http://96.31.83.86:8200/', '', 'War.png', 0, 0, 0, 1, '2011-11-11', 12928, 0, '2017-01-01 00:00:00', 0), \
('In-Memory-Of', '', 'http://96.31.83.86:8200/', '', 'In-Memory-Of.png', 0, 0, 0, 1, '2011-11-11', 12929, 0, '2017-01-01 00:00:00', 0), \
('MusikSystemet', '', 'http://96.31.83.86:8200/', '', 'MusikSystemet.png', 0, 0, 0, 1, '2011-11-11', 12930, 0, '2017-01-01 00:00:00', 0), \
('Virgin-Radio', '', 'http://96.31.83.86:8200/', 'http://www.virginradio.fr/', 'Virgin-Radio.png', 0, 0, 0, 1, '2011-11-11', 12931, 0, '2017-01-01 00:00:00', 0), \
('WRHIcom-Football-Stream-3', '', 'http://96.31.83.86:8200/', 'http://www.wrhi.com', 'WRHIcom-Football-Stream-3.png', 0, 0, 0, 1, '2011-11-11', 12932, 0, '2017-01-01 00:00:00', 0), \
('WPTL-920', 'WPTL (920 AM) is a radio station broadcasting a Country music format', 'http://96.31.83.86:8200/', 'http://wptlradio.com/', 'WPTL-920.png', 0, 0, 0, 1, '2011-11-11', 12933, 0, '2017-01-01 00:00:00', 0), \
('Fresh-1003', '', 'http://96.31.83.86:8200/', 'http://www.wnic.com/', 'Fresh-1003.png', 0, 0, 0, 1, '2011-11-11', 12934, 0, '2017-01-01 00:00:00', 0), \
('The-Source-1310', '', 'http://96.31.83.86:8200/', '', 'The-Source-1310.png', 0, 0, 0, 1, '2011-11-11', 12935, 0, '2017-01-01 00:00:00', 0), \
('KIA-FM-939', '', 'http://96.31.83.86:8200/', 'http://www.kiaifm.com/', 'KIA-FM-939.png', 0, 0, 0, 1, '2011-11-11', 12936, 0, '2017-01-01 00:00:00', 0), \
('Radio-Metro-1024', '', 'http://96.31.83.86:8200/', 'http://radiometro.ru/', 'Radio-Metro-1024.png', 0, 0, 0, 1, '2011-11-11', 12937, 0, '2017-01-01 00:00:00', 0), \
('The-Juan-MacLean', '', 'http://96.31.83.86:8200/', '', 'The-Juan-MacLean.png', 0, 0, 0, 1, '2011-11-11', 12938, 0, '2017-01-01 00:00:00', 0), \
('Weekly-Top-40', '', 'http://96.31.83.86:8200/', '', 'Weekly-Top-40.png', 0, 0, 0, 1, '2011-11-11', 12939, 0, '2017-01-01 00:00:00', 0), \
('Lazer-993', '', 'http://96.31.83.86:8200/', 'http://www.lazer993.com/', 'Lazer-993.png', 0, 0, 0, 1, '2011-11-11', 12940, 0, '2017-01-01 00:00:00', 0), \
('Thunder-102-1021', '', 'http://96.31.83.86:8200/', 'http://www.thunder102.com/', 'Thunder-102-1021.png', 0, 0, 0, 1, '2011-11-11', 12941, 0, '2017-01-01 00:00:00', 0), \
('American-Top-40', '', 'http://96.31.83.86:8200/', '', 'American-Top-40.png', 0, 0, 0, 1, '2011-11-11', 12942, 0, '2017-01-01 00:00:00', 0), \
('PRIMEtime', '', 'http://96.31.83.86:8200/', '', 'PRIMEtime.png', 0, 0, 0, 1, '2011-11-11', 12943, 0, '2017-01-01 00:00:00', 0), \
('K-953', '', 'http://96.31.83.86:8200/', 'http://www.k-musicradio.com/', 'K-953.png', 0, 0, 0, 1, '2011-11-11', 12944, 0, '2017-01-01 00:00:00', 0), \
('WXLO-1045', '', 'http://96.31.83.86:8200/', 'http://www.wxlo.com/', 'WXLO-1045.png', 0, 0, 0, 1, '2011-11-11', 12945, 0, '2017-01-01 00:00:00', 0), \
('A-different-idea', '', 'http://96.31.83.86:8200/', '', 'A-different-idea.png', 0, 0, 0, 1, '2011-11-11', 12946, 0, '2017-01-01 00:00:00', 0), \
('Yellow-Jackets-Update', '', 'http://96.31.83.86:8200/', '', 'Yellow-Jackets-Update.png', 0, 0, 0, 1, '2011-11-11', 12947, 0, '2017-01-01 00:00:00', 0), \
('WSOO-1230', '', 'http://96.31.83.86:8200/', 'http://www.1230wsoo.com/', 'WSOO-1230.png', 0, 0, 0, 1, '2011-11-11', 12948, 0, '2017-01-01 00:00:00', 0), \
('Sportstalk-790', '', 'http://96.31.83.86:8200/', 'http://www.sports790.com', 'Sportstalk-790.png', 0, 0, 0, 1, '2011-11-11', 12949, 0, '2017-01-01 00:00:00', 0), \
('MSA-Sports--PIHL-Hockey', '', 'http://96.31.83.86:8200/', '', 'MSA-Sports--PIHL-Hockey.png', 0, 0, 0, 1, '2011-11-11', 12950, 0, '2017-01-01 00:00:00', 0), \
('Black-Dahlia-Murder-The', '', 'http://96.31.83.86:8200/', '', 'Black-Dahlia-Murder-The.png', 0, 0, 0, 1, '2011-11-11', 12951, 0, '2017-01-01 00:00:00', 0), \
('WATT-1240', '', 'http://96.31.83.86:8200/', 'http://www.radiomgb.com/watt/', 'WATT-1240.png', 0, 0, 0, 1, '2011-11-11', 12952, 0, '2017-01-01 00:00:00', 0), \
('Shabba-Ranks', '', 'http://96.31.83.86:8200/', '', 'Shabba-Ranks.png', 0, 0, 0, 1, '2011-11-11', 12953, 0, '2017-01-01 00:00:00', 0), \
('Hunt-Life-Outdoors-Show', '', 'http://96.31.83.86:8200/', '', 'Hunt-Life-Outdoors-Show.png', 0, 0, 0, 1, '2011-11-11', 12954, 0, '2017-01-01 00:00:00', 0), \
('WAKE-1500', '', 'http://96.31.83.86:8200/', 'http://www.wakeradio.com/', 'WAKE-1500.png', 0, 0, 0, 1, '2011-11-11', 12955, 0, '2017-01-01 00:00:00', 0), \
('Magic-104-1045', '', 'http://96.31.83.86:8200/', 'http://www.conwaymagic.com/', 'Magic-104-1045.png', 0, 0, 0, 1, '2011-11-11', 12956, 0, '2017-01-01 00:00:00', 0), \
('Human-Nation-FM', '', 'http://96.31.83.86:8200/', 'http://www.hnfm.de/', 'Human-Nation-FM.png', 0, 0, 0, 1, '2011-11-11', 12957, 0, '2017-01-01 00:00:00', 0), \
('Light-of-Life-Job', '', 'http://96.31.83.86:8200/', '', 'Light-of-Life-Job.png', 0, 0, 0, 1, '2011-11-11', 12958, 0, '2017-01-01 00:00:00', 0), \
('Sua-trilha-4x4', '', 'http://96.31.83.86:8200/', '', 'Sua-trilha-4x4.png', 0, 0, 0, 1, '2011-11-11', 12959, 0, '2017-01-01 00:00:00', 0), \
('Kristians-Desired-Time', '', 'http://96.31.83.86:8200/', '', 'Kristians-Desired-Time.png', 0, 0, 0, 1, '2011-11-11', 12960, 0, '2017-01-01 00:00:00', 0), \
('MOViN-1049', '', 'http://96.31.83.86:8200/', 'http://www.movin1049.com/', 'MOViN-1049.png', 0, 0, 0, 1, '2011-11-11', 12961, 0, '2017-01-01 00:00:00', 0), \
('Big-Band-And-Trad', '', 'http://96.31.83.86:8200/', '', 'Big-Band-And-Trad.png', 0, 0, 1, 1, '2011-11-11', 12962, 0, '2011-10-14 16:04:17', 0), \
('WPEH-FM-921', '', 'http://96.31.83.86:8200/', '', 'WPEH-FM-921.png', 0, 0, 0, 1, '2011-11-11', 12963, 0, '2017-01-01 00:00:00', 0), \
('Hardy-and-Trupiano', '', 'http://96.31.83.86:8200/', '', 'Hardy-and-Trupiano.png', 0, 0, 0, 1, '2011-11-11', 12964, 0, '2017-01-01 00:00:00', 0), \
('KPRI-1021', 'KPRI-FM is owned by two guys.  Thats it. No big corporation', 'http://96.31.83.86:8200/', 'http://www.kprifm.com/', 'KPRI-1021.png', 0, 0, 0, 1, '2011-11-11', 12965, 0, '2017-01-01 00:00:00', 0), \
('Fun-Radio-Top-20', '24-hour stream 20 songs fun radio.', 'http://96.31.83.86:8200/', 'http://www.funradio.sk/', 'Fun-Radio-Top-20.png', 0, 0, 0, 1, '2011-11-11', 12966, 0, '2017-01-01 00:00:00', 0), \
('The-Sheep-540', '', 'http://96.31.83.86:8200/', 'http://www.wgth.net/', 'The-Sheep-540.png', 0, 0, 0, 1, '2011-11-11', 12967, 0, '2017-01-01 00:00:00', 0), \
('SODRE-CX6-R-Clasica-989', '', 'http://96.31.83.86:8200/', 'http://www.sodre.gub.uy/', 'SODRE-CX6-R-Clasica-989.png', 0, 0, 0, 1, '2011-11-11', 12968, 0, '2017-01-01 00:00:00', 0), \
('96-FM-961', '', 'http://96.31.83.86:8200/', 'http://www.96fm.com.au/', '96-FM-961.png', 0, 0, 0, 1, '2011-11-11', 12969, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Temple-Times', '', 'http://96.31.83.86:8200/', '', 'Sunday-Temple-Times.png', 0, 0, 0, 1, '2011-11-11', 12970, 0, '2017-01-01 00:00:00', 0), \
('Judas-Priest', '', 'http://96.31.83.86:8200/', '', 'Judas-Priest.png', 0, 0, 0, 1, '2011-11-11', 12971, 0, '2017-01-01 00:00:00', 0), \
('WPTF-680', '', 'http://96.31.83.86:8200/', 'http://www.wptf.com/', 'WPTF-680.png', 0, 0, 0, 1, '2011-11-11', 12972, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Indie-Rock', 'For discovering brand-new independent music and hearing stuff you may have missed, by emerging and established college and indie rock artists', 'http://96.31.83.86:8200/', 'http://www.radioio.com/channels/indie-rock', 'RadioIO-Indie-Rock.png', 0, 0, 0, 1, '2011-11-11', 12973, 0, '2017-01-01 00:00:00', 0), \
('Jazz-at-Dawn', '', 'http://96.31.83.86:8200/', '', 'Jazz-at-Dawn.png', 0, 0, 0, 1, '2011-11-11', 12974, 0, '2017-01-01 00:00:00', 0), \
('Dada-Life', '', 'http://96.31.83.86:8200/', '', 'Dada-Life.png', 0, 0, 0, 1, '2011-11-11', 12975, 0, '2017-01-01 00:00:00', 0), \
('Joes-Jazz', '', 'http://96.31.83.86:8200/', '', 'Joes-Jazz.png', 0, 0, 0, 1, '2011-11-11', 12976, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1230', '', 'http://96.31.83.86:8200/', 'http://espnfay.com/', 'ESPN-Radio-1230.png', 0, 0, 0, 1, '2011-11-11', 12977, 0, '2017-01-01 00:00:00', 0), \
('Coco-Montoya', '', 'http://96.31.83.86:8200/', '', 'Coco-Montoya.png', 0, 0, 0, 1, '2011-11-11', 12978, 0, '2017-01-01 00:00:00', 0), \
('DI-Lounge', 'Sit back and enjoy the lounge grooves!', 'http://96.31.83.86:8200/', 'http://www.di.fm/lounge', 'DI-Lounge.png', 0, 0, 0, 1, '2011-11-11', 12979, 0, '2017-01-01 00:00:00', 0), \
('SportsLine-Radio-Network', '', 'http://96.31.83.86:8200/', 'http://slrnradiosports.com/', 'SportsLine-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 12980, 0, '2017-01-01 00:00:00', 0), \
('WOKV-690', '', 'http://96.31.83.86:8200/', 'http://www.wokv.com/', 'WOKV-690.png', 0, 0, 0, 1, '2011-11-11', 12981, 0, '2017-01-01 00:00:00', 0), \
('Lunch-with-Lewi-McKirdy', '', 'http://96.31.83.86:8200/', '', 'Lunch-with-Lewi-McKirdy.png', 0, 0, 0, 1, '2011-11-11', 12982, 0, '2017-01-01 00:00:00', 0), \
('LBC-973', '', 'http://96.31.83.86:8200/', 'http://www.lbc.co.uk/', 'LBC-973.png', 0, 0, 0, 1, '2011-11-11', 12983, 0, '2017-01-01 00:00:00', 0), \
('Radio-Carlin', '', 'http://96.31.83.86:8200/', 'http://www.remembercarlin.com/', 'Radio-Carlin.png', 0, 0, 0, 1, '2011-11-11', 12984, 0, '2017-01-01 00:00:00', 0), \
('Racing-Ahead', '', 'http://96.31.83.86:8200/', '', 'Racing-Ahead.png', 0, 0, 0, 1, '2011-11-11', 12985, 0, '2017-01-01 00:00:00', 0), \
('La-Gran-D-1340', 'La GranD 1340 AM es la estaci&#243;n de los Hits de Regional Mexicana con el Piolin por la Ma&#241;ana.', 'http://96.31.83.86:8200/', 'http://www.lagrand1340kc.com', 'La-Gran-D-1340.png', 0, 0, 0, 1, '2011-11-11', 12986, 0, '2017-01-01 00:00:00', 0), \
('Alabama-Crimson-Tide-at-Ole-Miss-Rebels-Oct-15-2011', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 12987, 0, '2017-01-01 00:00:00', 0), \
('KCBF-820', '', 'http://96.31.83.86:8200/', 'http://www.820sports.com/', 'KCBF-820.png', 0, 0, 0, 1, '2011-11-11', 12988, 0, '2017-01-01 00:00:00', 0), \
('Indiana-Hoosiers-at-Iowa-Hawkeyes-Oct-22-2011', '', 'http://96.31.83.86:8200/', '', 'Indiana-Hoosiers-at-Iowa-Hawkeyes-Oct-22-2011.png', 0, 0, 0, 1, '2011-11-11', 12989, 0, '2017-01-01 00:00:00', 0), \
('Q1019', '', 'http://96.31.83.86:8200/', 'http://www.q1019.com/', 'Q1019.png', 0, 0, 0, 1, '2011-11-11', 12990, 0, '2017-01-01 00:00:00', 0), \
('WTSN-Sunday-Morning-Information-Center', '', 'http://96.31.83.86:8200/', '', 'WTSN-Sunday-Morning-Information-Center.png', 0, 0, 0, 1, '2011-11-11', 12991, 0, '2017-01-01 00:00:00', 0), \
('Lets-Talk-About-Jesus', '', 'http://96.31.83.86:8200/', '', 'Lets-Talk-About-Jesus.png', 0, 0, 0, 1, '2011-11-11', 12992, 0, '2017-01-01 00:00:00', 0), \
('Gods-Living-Word', '', 'http://96.31.83.86:8200/', '', 'Gods-Living-Word.png', 0, 0, 0, 1, '2011-11-11', 12993, 0, '2017-01-01 00:00:00', 0), \
('Arrow-FM-1078', 'Arrow FM plays a selection of hit songs from the last forty years, and offers regular updates on local news, sport, travel and events.', 'http://96.31.83.86:8200/', 'http://www.arrowfm.co.uk/', 'Arrow-FM-1078.png', 0, 0, 0, 1, '2011-11-11', 12994, 0, '2017-01-01 00:00:00', 0), \
('Del-Amitri', '', 'http://96.31.83.86:8200/', '', 'Del-Amitri.png', 0, 0, 0, 1, '2011-11-11', 12995, 0, '2017-01-01 00:00:00', 0), \
('Easy-Network-987', 'Easy Network , storica emittente di Veneto e Friuli Venezia Giulia, grazie alle sua diffusione capillare e alla sua programmazione musicale', 'http://96.31.83.86:8200/', 'http://www.easynetwork.fm/', 'Easy-Network-987.png', 0, 0, 0, 1, '2011-11-11', 12996, 0, '2017-01-01 00:00:00', 0), \
('KSUE-Religious-Programming', '', 'http://96.31.83.86:8200/', '', 'KSUE-Religious-Programming.png', 0, 0, 0, 1, '2011-11-11', 12997, 0, '2017-01-01 00:00:00', 0), \
('RFD-Illinois', '', 'http://96.31.83.86:8200/', '', 'RFD-Illinois.png', 0, 0, 0, 1, '2011-11-11', 12998, 0, '2017-01-01 00:00:00', 0), \
('Houndog-Radio', '', 'http://96.31.83.86:8200/', 'http://www.hounddogradio.net/', 'Houndog-Radio.png', 0, 0, 0, 1, '2011-11-11', 12999, 0, '2017-01-01 00:00:00', 0), \
('KCOX-1350', '', 'http://96.31.83.86:8200/', 'http://www.1027ktxj.com/', 'KCOX-1350.png', 0, 0, 0, 1, '2011-11-11', 13001, 0, '2017-01-01 00:00:00', 0), \
('WJMA-FM-1031', '', 'http://96.31.83.86:8200/', 'http://www.wjmafm.com/', 'WJMA-FM-1031.png', 0, 0, 0, 1, '2011-11-11', 13002, 0, '2017-01-01 00:00:00', 0), \
('WABH-1380', '', 'http://96.31.83.86:8200/', 'http://www.1380wabh.com/', 'WABH-1380.png', 0, 0, 0, 1, '2011-11-11', 13003, 0, '2017-01-01 00:00:00', 0), \
('Virgin-Radio-Pop-Rock', '', 'http://96.31.83.86:8200/', 'http://www.virginradio.fr', 'Virgin-Radio-Pop-Rock.png', 0, 0, 0, 1, '2011-11-11', 13004, 0, '2017-01-01 00:00:00', 0), \
('Burning-Spear', '', 'http://96.31.83.86:8200/', '', 'Burning-Spear.png', 0, 0, 0, 1, '2011-11-11', 13005, 0, '2017-01-01 00:00:00', 0), \
('Funeral-Party', '', 'http://96.31.83.86:8200/', '', 'Funeral-Party.png', 0, 0, 0, 1, '2011-11-11', 13006, 0, '2017-01-01 00:00:00', 0), \
('NonStopNeoncom', 'A radio station from the UK playing Brighter Music from the 80s &amp; 90s.', 'http://96.31.83.86:8200/', 'http://www.nonstopneon.com/', 'NonStopNeoncom.png', 0, 0, 0, 1, '2011-11-11', 13007, 0, '2017-01-01 00:00:00', 0), \
('Modern-Music-and-More', '', 'http://96.31.83.86:8200/', '', 'Modern-Music-and-More.png', 0, 0, 0, 1, '2011-11-11', 13008, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Fantasy-Focus-Basketball-Podcast', '', 'http://96.31.83.86:8200/', '', 'ESPN-Fantasy-Focus-Basketball-Podcast.png', 0, 0, 0, 1, '2011-11-11', 13009, 0, '2017-01-01 00:00:00', 0), \
('KWSN-Infomercial', '', 'http://96.31.83.86:8200/', '', 'KWSN-Infomercial.png', 0, 0, 0, 1, '2011-11-11', 13010, 0, '2017-01-01 00:00:00', 0), \
('Radio-64', '', 'http://96.31.83.86:8200/', 'http://www.radio64fm.fr/', 'Radio-64.png', 0, 0, 0, 1, '2011-11-11', 13011, 0, '2017-01-01 00:00:00', 0), \
('Clay-JD-Walker', '', 'http://96.31.83.86:8200/', '', 'Clay-JD-Walker.png', 0, 0, 0, 1, '2011-11-11', 13012, 0, '2017-01-01 00:00:00', 0), \
('The-Touch-923', '', 'http://96.31.83.86:8200/', 'http://www.carolinatouch.com/', 'The-Touch-923.png', 0, 0, 0, 1, '2011-11-11', 13013, 0, '2017-01-01 00:00:00', 0), \
('George-Strait', '', 'http://96.31.83.86:8200/', '', 'George-Strait.png', 0, 0, 0, 1, '2011-11-11', 13014, 0, '2017-01-01 00:00:00', 0), \
('Groovin-1580', '', 'http://96.31.83.86:8200/', 'http://www.groovin1580.com/', 'Groovin-1580.png', 0, 0, 0, 1, '2011-11-11', 13015, 0, '2017-01-01 00:00:00', 0), \
('Motor-Directo', '', 'http://96.31.83.86:8200/', '', 'Motor-Directo.png', 0, 0, 0, 1, '2011-11-11', 13016, 0, '2017-01-01 00:00:00', 0), \
('WTJK-1380', '', 'http://96.31.83.86:8200/', 'http://www.espn1380.com/', 'WTJK-1380.png', 0, 0, 0, 1, '2011-11-11', 13017, 0, '2017-01-01 00:00:00', 0), \
('WGAR-FM-995', '', 'http://96.31.83.86:8200/', 'http://www.wgar.com/', 'WGAR-FM-995.png', 0, 0, 0, 1, '2011-11-11', 13018, 0, '2017-01-01 00:00:00', 0), \
('Star-951', '', 'http://96.31.83.86:8200/', 'http://www.wcdzradio.com/', 'Star-951.png', 0, 0, 0, 1, '2011-11-11', 13019, 0, '2017-01-01 00:00:00', 0), \
('K-Bay-945', '', 'http://96.31.83.86:8200/', 'http://www.kbay.com/', 'K-Bay-945.png', 0, 0, 0, 1, '2011-11-11', 13020, 0, '2017-01-01 00:00:00', 0), \
('KYCA-1490', '', 'http://96.31.83.86:8200/', 'http://www.kyca.info/kyca.php', 'KYCA-1490.png', 0, 0, 0, 1, '2011-11-11', 13021, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-995', '', 'http://96.31.83.86:8200/', 'http://www.995thewolf.com/', 'The-Wolf-995.png', 0, 0, 0, 1, '2011-11-11', 13022, 0, '2017-01-01 00:00:00', 0), \
('The-Sisters-of-Mercy', '', 'http://96.31.83.86:8200/', '', 'The-Sisters-of-Mercy.png', 0, 0, 0, 1, '2011-11-11', 13023, 0, '2017-01-01 00:00:00', 0), \
('All-Over-the-Shop', '', 'http://96.31.83.86:8200/', '', 'All-Over-the-Shop.png', 0, 0, 0, 1, '2011-11-11', 13024, 0, '2017-01-01 00:00:00', 0), \
('Joe-Lovano', '', 'http://96.31.83.86:8200/', '', 'Joe-Lovano.png', 0, 0, 0, 1, '2011-11-11', 13025, 0, '2017-01-01 00:00:00', 0), \
('Golds-Greatest-Hits', '', 'http://96.31.83.86:8200/', '', 'Golds-Greatest-Hits.png', 0, 0, 0, 1, '2011-11-11', 13026, 0, '2017-01-01 00:00:00', 0), \
('NASCAR-USA', '', 'http://96.31.83.86:8200/', '', 'NASCAR-USA.png', 0, 0, 0, 1, '2011-11-11', 13027, 0, '2017-01-01 00:00:00', 0), \
('CKUA-FM-949', '', 'http://96.31.83.86:8200/', 'http://www.ckua.com/', 'CKUA-FM-949.png', 0, 0, 0, 1, '2011-11-11', 13028, 0, '2017-01-01 00:00:00', 0), \
('Halloween-Radio', 'Scary spooky horror thrilling Halloween radio.', 'http://96.31.83.86:8200/', 'http://www.halloweenradio.net/', 'Halloween-Radio.png', 0, 0, 0, 1, '2011-11-11', 13029, 0, '2017-01-01 00:00:00', 0), \
('Kids-Public-Radio-Pipsqueaks', '', 'http://96.31.83.86:8200/', 'http://www.kidspublicradio.org/', 'Kids-Public-Radio-Pipsqueaks.png', 0, 0, 0, 1, '2011-11-11', 13030, 0, '2017-01-01 00:00:00', 0), \
('Peter-Bjorn-and-John', '', 'http://96.31.83.86:8200/', '', 'Peter-Bjorn-and-John.png', 0, 0, 0, 1, '2011-11-11', 13031, 0, '2017-01-01 00:00:00', 0), \
('WTIF-FM-1075', '', 'http://96.31.83.86:8200/', 'http://www.wtif1075.com', 'WTIF-FM-1075.png', 0, 0, 0, 1, '2011-11-11', 13032, 0, '2017-01-01 00:00:00', 0), \
('Atmosphere', '', 'http://96.31.83.86:8200/', '', 'Atmosphere.png', 0, 0, 0, 1, '2011-11-11', 13033, 0, '2017-01-01 00:00:00', 0), \
('Deluxe-Radio', 'DELUXE RADIO ist das erste Radioprogramm das Ihnen 24 Stunden Musik pur bringt. Ein einmaliger Mix aus Contemporary Jazz, soft Soul und einen Hauch coolem Swing.', 'http://96.31.83.86:8200/', 'http://www.radiodeluxe.de/', 'Deluxe-Radio.png', 0, 0, 0, 1, '2011-11-11', 13034, 0, '2017-01-01 00:00:00', 0), \
('AccuRadio-Adult-Alternative-Double-Latte', '', 'http://96.31.83.86:8200/', 'http://www.accuradio.com/classical/', '', 0, 0, 0, 1, '2011-11-11', 13035, 0, '2017-01-01 00:00:00', 0), \
('WEOL-Morning-Show', '', 'http://96.31.83.86:8200/', '', 'WEOL-Morning-Show.png', 0, 0, 0, 1, '2011-11-11', 13036, 0, '2017-01-01 00:00:00', 0), \
('Classy-959', '', 'http://96.31.83.86:8200/', 'http://midutahradio.com/kmgr', 'Classy-959.png', 0, 0, 0, 1, '2011-11-11', 13037, 0, '2017-01-01 00:00:00', 0), \
('Modern-Music-and-More', '', 'http://96.31.83.86:8200/', '', 'Modern-Music-and-More.png', 0, 0, 0, 1, '2011-11-11', 13038, 0, '2017-01-01 00:00:00', 0), \
('Tom-Harrell', '', 'http://96.31.83.86:8200/', '', 'Tom-Harrell.png', 0, 0, 0, 1, '2011-11-11', 13039, 0, '2017-01-01 00:00:00', 0), \
('Lostprophets', '', 'http://96.31.83.86:8200/', '', 'Lostprophets.png', 0, 0, 0, 1, '2011-11-11', 13040, 0, '2017-01-01 00:00:00', 0), \
('Tina-Cousins', '', 'http://96.31.83.86:8200/', '', 'Tina-Cousins.png', 0, 0, 0, 1, '2011-11-11', 13041, 0, '2017-01-01 00:00:00', 0), \
('Star-1007', 'STAR 100.7 Today&#39;s Variety', 'http://96.31.83.86:8200/', 'http://www.1007.com/', 'Star-1007.png', 0, 0, 0, 1, '2011-11-11', 13042, 0, '2017-01-01 00:00:00', 0), \
('Randy-Owen', '', 'http://96.31.83.86:8200/', '', 'Randy-Owen.png', 0, 0, 0, 1, '2011-11-11', 13043, 0, '2017-01-01 00:00:00', 0), \
('domradiode-9675', 'domradio.de - der Sender des Erzbistums K&#246;ln - sendet seit Pfingsten 2000 und ist der erste kirchliche Sender in offizieller', 'http://96.31.83.86:8200/', 'http://www.domradio.de/', 'domradiode-9675.png', 0, 0, 0, 1, '2011-11-11', 13044, 0, '2017-01-01 00:00:00', 0), \
('Baseball-Today', '', 'http://96.31.83.86:8200/', '', 'Baseball-Today.png', 0, 0, 0, 1, '2011-11-11', 13045, 0, '2017-01-01 00:00:00', 0), \
('Craig-Chaquico', '', 'http://96.31.83.86:8200/', '', 'Craig-Chaquico.png', 0, 0, 0, 1, '2011-11-11', 13046, 0, '2017-01-01 00:00:00', 0), \
('Columbus-Blue-Jackets', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 13047, 0, '2017-01-01 00:00:00', 0), \
('Majic-1057', 'Cleveland&#39;s Greatest Hits...&quot;Classic Hits&quot; station!', 'http://96.31.83.86:8200/', 'http://www.wmji.com/', 'Majic-1057.png', 0, 0, 0, 1, '2011-11-11', 13048, 0, '2017-01-01 00:00:00', 0), \
('Gym-Class-Heroes', '', 'http://96.31.83.86:8200/', '', 'Gym-Class-Heroes.png', 0, 0, 0, 1, '2011-11-11', 13049, 0, '2017-01-01 00:00:00', 0), \
('Matthew-Sweet', '', 'http://96.31.83.86:8200/', '', 'Matthew-Sweet.png', 0, 0, 0, 1, '2011-11-11', 13050, 0, '2017-01-01 00:00:00', 0), \
('Focus-Kultur', '', 'http://96.31.83.86:8200/', '', 'Focus-Kultur.png', 0, 0, 0, 1, '2011-11-11', 13051, 0, '2017-01-01 00:00:00', 0), \
('LEN', '', 'http://96.31.83.86:8200/', '', 'LEN.png', 0, 0, 0, 1, '2011-11-11', 13052, 0, '2017-01-01 00:00:00', 0), \
('The-Heavy', '', 'http://96.31.83.86:8200/', '', 'The-Heavy.png', 0, 0, 0, 1, '2011-11-11', 13053, 0, '2017-01-01 00:00:00', 0), \
('Jon-Linder-Show', '', 'http://96.31.83.86:8200/', '', 'Jon-Linder-Show.png', 0, 0, 0, 1, '2011-11-11', 13054, 0, '2017-01-01 00:00:00', 0), \
('BBC-Kent-967', '', 'http://96.31.83.86:8200/', 'http://www.bbc.co.uk/kent/', 'BBC-Kent-967.png', 0, 0, 1, 1, '2011-11-11', 13055, 0, '2011-10-14 16:03:12', 0), \
('Sunshine-FM-994', '', 'http://96.31.83.86:8200/', 'http://www.sunshinefm.hu/', 'Sunshine-FM-994.png', 0, 0, 0, 1, '2011-11-11', 13056, 0, '2017-01-01 00:00:00', 0), \
('Beenie-Man', '', 'http://96.31.83.86:8200/', '', 'Beenie-Man.png', 0, 0, 0, 1, '2011-11-11', 13057, 0, '2017-01-01 00:00:00', 0), \
('Redeemer-Lutheran-Church', '', 'http://96.31.83.86:8200/', '', 'Redeemer-Lutheran-Church.png', 0, 0, 0, 1, '2011-11-11', 13058, 0, '2017-01-01 00:00:00', 0), \
('The-Beautiful-South', '', 'http://96.31.83.86:8200/', '', 'The-Beautiful-South.png', 0, 0, 0, 1, '2011-11-11', 13059, 0, '2017-01-01 00:00:00', 0), \
('Martial-Arts-Legends', '', 'http://96.31.83.86:8200/', '', 'Martial-Arts-Legends.png', 0, 0, 0, 1, '2011-11-11', 13060, 0, '2017-01-01 00:00:00', 0), \
('-and-the-Mysterians', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 13061, 0, '2017-01-01 00:00:00', 0), \
('Mix-1007', '', 'http://96.31.83.86:8200/', 'http://mix100fm.com/', 'Mix-1007.png', 0, 0, 0, 1, '2011-11-11', 13062, 0, '2017-01-01 00:00:00', 0), \
('Radio-Activa', 'Estaci&#243;n de la Universidad del Sagrado Coraz&#243;n', 'http://96.31.83.86:8200/', 'http://www.sagrado.edu/radioactiva/index.htm', 'Radio-Activa.png', 0, 0, 0, 1, '2011-11-11', 13063, 0, '2017-01-01 00:00:00', 0), \
('AFN-News', '', 'http://96.31.83.86:8200/', 'http://myafn.dodmedia.osd.mil/News.aspx', 'AFN-News.png', 0, 0, 0, 1, '2011-11-11', 13064, 0, '2017-01-01 00:00:00', 0), \
('Chicago-Bears-at-Detroit-Lions-Oct-10-2011', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 13065, 0, '2017-01-01 00:00:00', 0), \
('World-Cup-Drivetime', '', 'http://96.31.83.86:8200/', '', 'World-Cup-Drivetime.png', 0, 0, 0, 1, '2011-11-11', 13066, 0, '2017-01-01 00:00:00', 0), \
('The-John-Kincade-Show', '', 'http://96.31.83.86:8200/', '', 'The-John-Kincade-Show.png', 0, 0, 0, 1, '2011-11-11', 13067, 0, '2017-01-01 00:00:00', 0), \
('Bob-FM-1039', '', 'http://96.31.83.86:8200/', 'http://www.1039bobfm.com/', 'Bob-FM-1039.png', 0, 0, 0, 1, '2011-11-11', 13068, 0, '2017-01-01 00:00:00', 0), \
('Five', '', 'http://96.31.83.86:8200/', '', 'Five.png', 0, 0, 0, 1, '2011-11-11', 13069, 0, '2017-01-01 00:00:00', 0), \
('WARF-1350', '', 'http://96.31.83.86:8200/', 'http://www.sportsradio1350.com/main.html', 'WARF-1350.png', 0, 0, 0, 1, '2011-11-11', 13070, 0, '2017-01-01 00:00:00', 0), \
('Deerhunter', '', 'http://96.31.83.86:8200/', '', 'Deerhunter.png', 0, 0, 0, 1, '2011-11-11', 13071, 0, '2017-01-01 00:00:00', 0), \
('Towards-London-2012-Olympic-and-Paralympic-programme', '', 'http://96.31.83.86:8200/', '', 'Towards-London-2012-Olympic-and-Paralympic-programme.png', 0, 0, 0, 1, '2011-11-11', 13072, 0, '2017-01-01 00:00:00', 0), \
('KCTC-1320', '', 'http://96.31.83.86:8200/', 'http://www.espn1320.net', 'KCTC-1320.png', 0, 0, 0, 1, '2011-11-11', 13073, 0, '2017-01-01 00:00:00', 0), \
('WDMG-860', '', 'http://96.31.83.86:8200/', '', 'WDMG-860.png', 0, 0, 0, 1, '2011-11-11', 13074, 0, '2017-01-01 00:00:00', 0), \
('Ace-Frehley', '', 'http://96.31.83.86:8200/', '', 'Ace-Frehley.png', 0, 0, 0, 1, '2011-11-11', 13075, 0, '2017-01-01 00:00:00', 0), \
('xitos-FM-999', '', 'http://96.31.83.86:8200/', 'http://www.exitosfm.com', 'xitos-FM-999.png', 0, 0, 0, 1, '2011-11-11', 13076, 0, '2017-01-01 00:00:00', 0), \
('Outdoors-This-Week', '', 'http://96.31.83.86:8200/', '', 'Outdoors-This-Week.png', 0, 0, 0, 1, '2011-11-11', 13077, 0, '2017-01-01 00:00:00', 0), \
('Country-1065', '', 'http://96.31.83.86:8200/', 'http://www.wyrk.com/', 'Country-1065.png', 0, 0, 0, 1, '2011-11-11', 13078, 0, '2017-01-01 00:00:00', 0), \
('KLOG-1490', '', 'http://96.31.83.86:8200/', 'http://www.klog.com/', 'KLOG-1490.png', 0, 0, 0, 1, '2011-11-11', 13079, 0, '2017-01-01 00:00:00', 0), \
('Terror', '', 'http://96.31.83.86:8200/', '', 'Terror.png', 0, 0, 0, 1, '2011-11-11', 13080, 0, '2017-01-01 00:00:00', 0), \
('Absolute-Hitz', '', 'http://96.31.83.86:8200/', 'http://AbsoluteHitz.com', 'Absolute-Hitz.png', 0, 0, 0, 1, '2011-11-11', 13081, 0, '2017-01-01 00:00:00', 0), \
('PartyMix', '', 'http://96.31.83.86:8200/', '', 'PartyMix.png', 0, 0, 0, 1, '2011-11-11', 13082, 0, '2017-01-01 00:00:00', 0), \
('RFI-Romania-935', '', 'http://96.31.83.86:8200/', 'http://www.rfi.ro/', 'RFI-Romania-935.png', 0, 0, 0, 1, '2011-11-11', 13083, 0, '2017-01-01 00:00:00', 0), \
('The-Knowledge-Show-Live', 'Live radio broadcast from Baltimore, MD touching on topics of conspiracy, education, religious topics, family discussions, mature topics, psychology, politics, and more. The show is in a mixed format ', 'http://96.31.83.86:8200/', 'http://www.knowledgeshowlive.com', 'The-Knowledge-Show-Live.png', 0, 0, 0, 1, '2011-11-11', 13084, 0, '2017-01-01 00:00:00', 0), \
('PulsRadio-Dance-And-Trance', '', 'http://96.31.83.86:8200/', 'http://www.pulsradio.com/', 'PulsRadio-Dance-And-Trance.png', 0, 0, 0, 1, '2011-11-11', 13085, 0, '2017-01-01 00:00:00', 0), \
('Jamiroquai', '', 'http://96.31.83.86:8200/', '', 'Jamiroquai.png', 0, 0, 0, 1, '2011-11-11', 13086, 0, '2017-01-01 00:00:00', 0), \
('KYYK-983', '', 'http://96.31.83.86:8200/', 'http://www.youreasttexas.com/', 'KYYK-983.png', 0, 0, 0, 1, '2011-11-11', 13087, 0, '2017-01-01 00:00:00', 0), \
('Muddy-Waters', '', 'http://96.31.83.86:8200/', '', 'Muddy-Waters.png', 0, 0, 0, 1, '2011-11-11', 13088, 0, '2017-01-01 00:00:00', 0), \
('Fairchild-Radio-1470', '', 'http://96.31.83.86:8200/', 'http://www.am1470.com/', 'Fairchild-Radio-1470.png', 0, 0, 0, 1, '2011-11-11', 13089, 0, '2017-01-01 00:00:00', 0), \
('Light-of-Life-Gods-plan-for-the-future', '', 'http://96.31.83.86:8200/', '', 'Light-of-Life-Gods-plan-for-the-future.png', 0, 0, 0, 1, '2011-11-11', 13090, 0, '2017-01-01 00:00:00', 0), \
('Boerne-Church-Of-Christ', '', 'http://96.31.83.86:8200/', '', 'Boerne-Church-Of-Christ.png', 0, 0, 0, 1, '2011-11-11', 13091, 0, '2017-01-01 00:00:00', 0), \
('KRLT-939', '', 'http://96.31.83.86:8200/', 'http://www.krltfm.com/', 'KRLT-939.png', 0, 0, 0, 1, '2011-11-11', 13092, 0, '2017-01-01 00:00:00', 0), \
('Dizzy-Gillespie-with-Sonny-Rollins-and-Sonny-Stitt', '', 'http://96.31.83.86:8200/', '', '', 0, 0, 0, 1, '2011-11-11', 13093, 0, '2017-01-01 00:00:00', 0), \
('Parachute-Club', '', 'http://96.31.83.86:8200/', '', 'Parachute-Club.png', 0, 0, 0, 1, '2011-11-11', 13094, 0, '2017-01-01 00:00:00', 0), \
('1057-The-Fan', '105.7 The Fan is a Sports Radio station based in Baltimore, MD.  105.7 The Fan can be heard at radio.com', 'http://96.31.83.86:8200/', 'http://www.1057thefan.com/', '1057-The-Fan.png', 0, 0, 0, 1, '2011-11-11', 13095, 0, '2017-01-01 00:00:00', 0), \
('Lloyd', '', 'http://96.31.83.86:8200/', '', 'Lloyd.png', 0, 0, 0, 1, '2011-11-11', 13096, 0, '2017-01-01 00:00:00', 0), \
('News-Radio-1260', '', 'http://96.31.83.86:8200/', 'http://www.wxce1260.com/', 'News-Radio-1260.png', 0, 0, 0, 1, '2011-11-11', 13097, 0, '2017-01-01 00:00:00', 0), \
('Fools-Gold', '', 'http://96.31.83.86:8200/', '', 'Fools-Gold.png', 0, 0, 0, 1, '2011-11-11', 13098, 0, '2017-01-01 00:00:00', 0), \
('999-Radio', '', 'http://96.31.83.86:8200/', 'http://www.fm999.info/', '999-Radio.png', 0, 0, 0, 1, '2011-11-11', 13099, 0, '2017-01-01 00:00:00', 0), \
('Lonestar', '', 'http://96.31.83.86:8200/', '', 'Lonestar.png', 0, 0, 0, 1, '2011-11-11', 13101, 0, '2017-01-01 00:00:00', 0), \
('Back-on-Basin-Street', '', 'http://96.31.83.86:8200/', '', 'Back-on-Basin-Street.png', 0, 0, 1, 1, '2011-11-11', 13102, 0, '2011-10-14 16:02:06', 0), \
('Joseph-Level', '', 'http://96.31.83.86:8200/', '', 'Joseph-Level.png', 0, 0, 0, 1, '2011-11-11', 13103, 0, '2017-01-01 00:00:00', 0), \
('Golden-Radio', 'Golden Radio', 'http://96.31.83.86:8200/', 'http://www.goldenradio.it/', 'Golden-Radio.png', 0, 0, 0, 1, '2011-11-11', 13104, 0, '2017-01-01 00:00:00', 0), \
('AFR-Talk-917', 'American Family Radio (AFR) is a network of more than 180 radio stations broadcasting Christian-oriented programming to over 40 states.', 'http://96.31.83.86:8200/', 'http://www.afr.net/', 'AFR-Talk-917.png', 0, 0, 0, 1, '2011-11-11', 13105, 0, '2017-01-01 00:00:00', 0), \
('The-Rezny-Wealth-Report', '', 'http://96.31.83.86:8200/', '', 'The-Rezny-Wealth-Report.png', 0, 0, 0, 1, '2011-11-11', 13106, 0, '2017-01-01 00:00:00', 0), \
('Faith-Evans', '', 'http://96.31.83.86:8200/', '', 'Faith-Evans.png', 0, 0, 0, 1, '2011-11-11', 13107, 0, '2017-01-01 00:00:00', 0), \
('WKZO-590', '', 'http://96.31.83.86:8200/', 'http://www.wkzo.com/', 'WKZO-590.png', 0, 0, 0, 1, '2011-11-11', 13108, 0, '2017-01-01 00:00:00', 0), \
('Radio-La-Chevere-1009', '', 'http://96.31.83.86:8200/', 'http://www.radiolachevere.info/', 'Radio-La-Chevere-1009.png', 0, 0, 0, 1, '2011-11-11', 13109, 0, '2017-01-01 00:00:00', 0), \
('Smooth-937', '', 'http://96.31.83.86:8200/', 'http://www.kjzy.com/', 'Smooth-937.png', 0, 0, 0, 1, '2011-11-11', 13110, 0, '2017-01-01 00:00:00', 0), \
('WKUL-921', 'WKUL (92.1 FM, &quot;Country K-92&quot;) is a radio station licensed to serve Cullman, Alabama. The station is owned by Jonathan Christian Corp.', 'http://96.31.83.86:8200/', 'http://www.wkul.com/', 'WKUL-921.png', 0, 0, 0, 1, '2011-11-11', 13111, 0, '2017-01-01 00:00:00', 0), \
('WSNJ-1240', '', 'http://96.31.83.86:8200/', 'http://www.wsnjam.com/', 'WSNJ-1240.png', 0, 0, 0, 1, '2011-11-11', 13112, 0, '2017-01-01 00:00:00', 0), \
('Nirvana', '', 'http://96.31.83.86:8200/', '', 'Nirvana.png', 0, 0, 0, 1, '2011-11-11', 13113, 0, '2017-01-01 00:00:00', 0), \
('The-Gospel-Spin', '', 'http://96.31.83.86:8200/', '', 'The-Gospel-Spin.png', 0, 0, 0, 1, '2011-11-11', 13114, 0, '2017-01-01 00:00:00', 0), \
('Aa', '', 'http://96.31.83.86:8200/', '', 'Aa.png', 0, 0, 0, 1, '2011-11-11', 13115, 0, '2017-01-01 00:00:00', 0), \
('1010-WINS', '1010 WINS is a News radio station based in New York, NY.  1010 WINS can be heard at radio.com', 'http://96.31.83.86:8200/', 'http://www.wins.com/', '1010-WINS.png', 0, 0, 0, 1, '2011-11-11', 13116, 0, '2017-01-01 00:00:00', 0), \
('Dive-Bar-Jukebox-Radio', '', 'http://96.31.83.86:8200/', 'http://www.charlestonillustrated.com/radio.htm', 'Dive-Bar-Jukebox-Radio.png', 0, 0, 0, 1, '2011-11-11', 13117, 0, '2017-01-01 00:00:00', 0), \
('Jimmie-Rodgers', '', 'http://96.31.83.86:8200/', '', 'Jimmie-Rodgers.png', 0, 0, 0, 1, '2011-11-11', 13118, 0, '2017-01-01 00:00:00', 0), \
('Disturbed', '', 'http://96.31.83.86:8200/', '', 'Disturbed.png', 0, 0, 0, 1, '2011-11-11', 13119, 0, '2017-01-01 00:00:00', 0), \
('Tell-It-Again', '', 'http://96.31.83.86:8200/', '', 'Tell-It-Again.png', 0, 0, 0, 1, '2011-11-11', 13120, 0, '2017-01-01 00:00:00', 0), \
('Jazz-Profiles', '', 'http://96.31.83.86:8200/', '', 'Jazz-Profiles.png', 0, 0, 0, 1, '2011-11-11', 13121, 0, '2017-01-01 00:00:00', 0), \
('971-The-Ticket', 'Detroit&#39;s #1 Sports Station', 'http://96.31.83.86:8200/', 'http://www.971theticket.com', '971-The-Ticket.png', 0, 0, 0, 1, '2011-11-11', 13122, 0, '2017-01-01 00:00:00', 0), \
('Sounds-of-the-20th-Century', '', 'http://96.31.83.86:8200/', '', 'Sounds-of-the-20th-Century.png', 0, 0, 0, 1, '2011-11-11', 13123, 0, '2017-01-01 00:00:00', 0), \
('WVHU-800', '', 'http://96.31.83.86:8200/', 'http://www.800wvhu.com/', 'WVHU-800.png', 0, 0, 0, 1, '2011-11-11', 13124, 0, '2017-01-01 00:00:00', 0), \
('KLOE-730', '', 'http://96.31.83.86:8200/', 'http://www.kloe.com/', 'KLOE-730.png', 0, 0, 0, 1, '2011-11-11', 13125, 0, '2017-01-01 00:00:00', 0), \
('WPKO-FM-983', 'WPKO-FM (98.3 FM) is an American radio station. It is programmed in a Hot AC radio format, and is almost entirely locally produced.', 'http://96.31.83.86:8200/', 'http://www.peakofohio.com/', 'WPKO-FM-983.png', 0, 0, 0, 1, '2011-11-11', 13127, 0, '2017-01-01 00:00:00', 0), \
('D-FM-902', '', 'http://96.31.83.86:8200/', 'http://www.dfm.ee/', 'D-FM-902.png', 0, 0, 0, 1, '2011-11-11', 13128, 0, '2017-01-01 00:00:00', 0), \
('Stevie-Storm', '', 'http://96.31.83.86:8200/', '', 'Stevie-Storm.png', 0, 0, 0, 1, '2011-11-11', 13129, 0, '2017-01-01 00:00:00', 0), \
('K971', '', 'http://96.31.83.86:8200/', 'http://www.k97fm.com/', 'K971.png', 0, 0, 0, 1, '2011-11-11', 13130, 0, '2017-01-01 00:00:00', 0), \
('Raw-FM-876', '', 'http://96.31.83.86:8200/', 'http://www.rawfm.com.au/', 'Raw-FM-876.png', 0, 0, 0, 1, '2011-11-11', 13131, 0, '2017-01-01 00:00:00', 0), \
('The-Stone-Roses', '', 'http://96.31.83.86:8200/', '', 'The-Stone-Roses.png', 0, 0, 0, 1, '2011-11-11', 13132, 0, '2017-01-01 00:00:00', 0), \
('Bunker-to-Bunker', '', 'http://96.31.83.86:8200/', '', 'Bunker-to-Bunker.png', 0, 0, 0, 1, '2011-11-11', 13133, 0, '2017-01-01 00:00:00', 0), \
('Tambayan-1019', 'DWRR-FM, branded as Tambayan 101.9, is the flagship  FM station of the CBN Corporation in the Philippines.', 'http://96.31.83.86:8200/', 'http://www.tambayan1019.com/', 'Tambayan-1019.png', 0, 0, 0, 1, '2011-11-11', 13134, 0, '2017-01-01 00:00:00', 0), \
('Tech-Nation', '', 'http://96.31.83.86:8200/', '', 'Tech-Nation.png', 0, 0, 0, 1, '2011-11-11', 13135, 0, '2017-01-01 00:00:00', 0), \
('Radio-SLR-1010', '', 'http://96.31.83.86:8200/', 'http://www.radioslr.dk/', 'Radio-SLR-1010.png', 0, 0, 0, 1, '2011-11-11', 13136, 0, '2017-01-01 00:00:00', 0), \
('The-Weekend-Gardener', '', 'http://96.31.83.86:8200/', '', 'The-Weekend-Gardener.png', 0, 0, 0, 1, '2011-11-11', 13137, 0, '2017-01-01 00:00:00', 0), \
('Diffuser', '', 'http://96.31.83.86:8200/', '', 'Diffuser.png', 0, 0, 0, 1, '2011-11-11', 13138, 0, '2017-01-01 00:00:00', 0), \
('Q100-997', '', 'http://96.31.83.86:8200/', 'http://www.allthehitsq100.com/', 'Q100-997.png', 0, 0, 0, 1, '2011-11-11', 13139, 0, '2017-01-01 00:00:00', 0), \
('1027FM', '', 'http://96.31.83.86:8200/', '', '1027FM.png', 0, 0, 0, 1, '2011-11-11', 13140, 0, '2017-01-01 00:00:00', 0), \
('True-Oldies-1063', '', 'http://96.31.83.86:8200/', 'http://www.trueoldies1063.com/', 'True-Oldies-1063.png', 0, 0, 0, 1, '2011-11-11', 13141, 0, '2017-01-01 00:00:00', 0), \
('Kosher-Metal', 'Online Metal Radio - Uncut and Uncensored m/', 'http://96.31.83.86:8200/', 'http://loudcaster.com/channels/589-kosher-metal', 'Kosher-Metal.png', 0, 0, 0, 1, '2011-11-11', 13142, 0, '2017-01-01 00:00:00', 0), \
('WPSL-1590', 'WPSL AM-1590 Port St. Lucie, Florida, The Talk of the Treasure Coast.', 'http://96.31.83.86:8200/', 'http://www.wpsl.com/', 'WPSL-1590.png', 0, 0, 0, 1, '2011-11-11', 13143, 0, '2017-01-01 00:00:00', 0), \
('Smooth-Radio-Glasgow-1052', '', 'http://96.31.83.86:8200/', 'http://www.smoothradioglasgow.co.uk/', 'Smooth-Radio-Glasgow-1052.png', 0, 0, 0, 1, '2011-11-11', 13144, 0, '2017-01-01 00:00:00', 0), \
('KSKK-947', '', 'http://96.31.83.86:8200/', 'http://www.kkradionetwork.com/', 'KSKK-947.png', 0, 0, 0, 1, '2011-11-11', 13145, 0, '2017-01-01 00:00:00', 0), \
('No-Holds-Barred-Radio', '', 'http://96.31.83.86:8200/', 'http://www.nhbradio.com/', 'No-Holds-Barred-Radio.png', 0, 0, 0, 1, '2011-11-11', 13146, 0, '2017-01-01 00:00:00', 0), \
('KPND-953', '', 'http://96.31.83.86:8200/', 'http://www.953kpnd.com/', 'KPND-953.png', 0, 0, 0, 1, '2011-11-11', 13147, 0, '2017-01-01 00:00:00', 0), \
('Georgia-Tech-Yellow-Jackets-at-Virginia-Cavaliers-Oct-15-2011', '', 'http://96.31.83.86:8200/', '', 'Georgia-Tech-Yellow-Jackets-at-Virginia-Cavaliers-Oct-15-2011.png', 0, 0, 0, 1, '2011-11-11', 13148, 0, '2017-01-01 00:00:00', 0), \
('Mary-Anne-Hobbs', '', 'http://96.31.83.86:8200/', '', 'Mary-Anne-Hobbs.png', 0, 0, 0, 1, '2011-11-11', 13149, 0, '2017-01-01 00:00:00', 0), \
('KRCY-FM-967', '', 'http://96.31.83.86:8200/', 'http://www.maddog.net/', 'KRCY-FM-967.png', 0, 0, 0, 1, '2011-11-11', 13150, 0, '2017-01-01 00:00:00', 0), \
('Chris-Rea', '', 'http://96.31.83.86:8200/', '', 'Chris-Rea.png', 0, 0, 0, 1, '2011-11-11', 13151, 0, '2017-01-01 00:00:00', 0), \
('Los-Angeles-Kings-Play-by-Play', '', 'http://96.31.83.86:8200/', '', 'Los-Angeles-Kings-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 13152, 0, '2017-01-01 00:00:00', 0), \
('The-Front-Row', '', 'http://96.31.83.86:8200/', '', 'The-Front-Row.png', 0, 0, 0, 1, '2011-11-11', 13153, 0, '2017-01-01 00:00:00', 0), \
('WGNB-893', '', 'http://96.31.83.86:8200/', 'http://www.mbn.org/GenMoody/default.asp?sectionid=ccdc3e516b72400bbf827edb0d31da4f', 'WGNB-893.png', 0, 0, 0, 1, '2011-11-11', 13154, 0, '2017-01-01 00:00:00', 0), \
('Get-1025', '', 'http://96.31.83.86:8200/', 'http://thisisclick.com/get/', 'Get-1025.png', 0, 0, 0, 1, '2011-11-11', 13155, 0, '2017-01-01 00:00:00', 0), \
('Brian-Mason', '', 'http://96.31.83.86:8200/', '', 'Brian-Mason.png', 0, 0, 0, 1, '2011-11-11', 13156, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-70s-Pop', 'Pure retro-pop, with all the 70s hits, one hit wonders, and forgotten guilty pleasures - all the music that used to make listening to the radio really fun and exciting.', 'http://96.31.83.86:8200/', 'http://www.radioio.com/channels/70s/pop', 'RadioIO-70s-Pop.png', 0, 0, 0, 1, '2011-11-11', 13157, 0, '2017-01-01 00:00:00', 0), \
('Praise-1047', '', 'http://96.31.83.86:8200/', 'http://praiserichmond.com/', 'Praise-1047.png', 0, 0, 0, 1, '2011-11-11', 13158, 0, '2017-01-01 00:00:00', 0), \
('Maysa', '', 'http://96.31.83.86:8200/', '', 'Maysa.png', 0, 0, 0, 1, '2011-11-11', 13159, 0, '2017-01-01 00:00:00', 0), \
('DR P4 København', '', 'http://live-icy.gss.dr.dk:8000/A/A08H.mp3', 'http://www.dr.dk/p4/kbh/', 'DR-P4-Koebenhavn-965.png', 0, 0, 0, 1, '2011-11-11', 13160, 0, '2017-01-01 00:00:00', 0), \
('MMA-Nation', '', 'http://96.31.83.86:8200/', '', 'MMA-Nation.png', 0, 0, 0, 1, '2011-11-11', 13161, 0, '2017-01-01 00:00:00', 0), \
('Big-Joe-Turner', '', 'http://96.31.83.86:8200/', '', 'Big-Joe-Turner.png', 0, 0, 0, 1, '2011-11-11', 13163, 0, '2017-01-01 00:00:00', 0), \
('Chris-Tomlin', '', 'http://96.31.83.86:8200/', '', 'Chris-Tomlin.png', 0, 0, 0, 1, '2011-11-11', 13164, 0, '2017-01-01 00:00:00', 0), \
('Esperanza-Spalding', '', 'http://96.31.83.86:8200/', '', 'Esperanza-Spalding.png', 0, 0, 1, 1, '2011-11-11', 13165, 0, '2011-10-14 16:07:17', 0), \
('KVNA-600', 'The only place in Flagstaff where you can hear late-breaking local news!', 'http://206.80.217.135:8088', 'http://www.myradioplace.com/am600/am600.htm', 'KVNA-600.png', 0, 0, 0, 1, '2011-11-11', 13166, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1000', '', 'http://206.80.217.135:8088', 'http://sports.espn.go.com/stations/espnradio1000/', 'ESPN-1000.png', 0, 0, 0, 1, '2011-11-11', 13167, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-Inspiration', '', 'http://206.80.217.135:8088', '', 'Sunday-Morning-Inspiration.png', 0, 0, 0, 1, '2011-11-11', 13168, 0, '2017-01-01 00:00:00', 0), \
('Avto-Radio-903', '', 'http://81.19.85.199/auto128.mp3', 'http://www.avtoradio.ru/', 'Avto-Radio-903.png', 0, 0, 0, 1, '2011-11-11', 13169, 0, '2017-01-01 00:00:00', 0), \
('Rosa-renditions-Special', '', 'http://81.19.85.199/auto128.mp3', '', 'Rosa-renditions-Special.png', 0, 0, 0, 1, '2011-11-11', 13170, 0, '2017-01-01 00:00:00', 0), \
('Draino-Radio', '', 'http://s4.myradiostream.com:22274/', 'http://drainoradio.com/', 'Draino-Radio.png', 0, 0, 0, 1, '2011-11-11', 13171, 0, '2017-01-01 00:00:00', 0), \
('Out-of-Bounds', '', 'http://s4.myradiostream.com:22274/', '', 'Out-of-Bounds.png', 0, 0, 0, 1, '2011-11-11', 13172, 0, '2017-01-01 00:00:00', 0), \
('The-Ump-730', '', 'mms://live.cumulusstreaming.com/WUMP-AM', 'http://www.730ump.com/', 'The-Ump-730.png', 0, 0, 0, 1, '2011-11-11', 13173, 0, '2017-01-01 00:00:00', 0), \
('KPUS-1045', 'Corpus Christi&#39;s Classic Rock Station.', 'mms://nick9.surfernetwork.com/KPUS', 'http://www.classicrock1045.com/', 'KPUS-1045.png', 0, 0, 0, 1, '2011-11-11', 13174, 0, '2017-01-01 00:00:00', 0), \
('Spirit-1059', 'SPIRIT 105.9, The Spirit of Austin, features Adult Contemporary Christian music; a popular music format that appeals to a wide range of listeners.', 'http://crista-kfmk.streamguys1.com/kfmkaacp.m3u', 'http://www.spirit1059.com/', 'Spirit-1059.png', 0, 0, 0, 1, '2011-11-11', 13176, 0, '2017-01-01 00:00:00', 0), \
('Heaven-887', '', 'http://stream2.nwrnetwork.com/KFBN-FM', 'http://www.kfbn.org/', 'Heaven-887.png', 0, 0, 0, 1, '2011-11-11', 13177, 0, '2017-01-01 00:00:00', 0), \
('Mix-96-961', '', 'http://stream2.nwrnetwork.com/KFBN-FM', 'http://www.totalradio.com/96x.htm', 'Mix-96-961.png', 0, 0, 0, 1, '2011-11-11', 13178, 0, '2017-01-01 00:00:00', 0), \
('WIGV-LP-965', '', 'mms://72.87.113.100/Renacer', 'http://radiorenacerri.com/', 'WIGV-LP-965.png', 0, 0, 0, 1, '2011-11-11', 13179, 0, '2017-01-01 00:00:00', 0), \
('Hallelujah-1043', '', 'mms://72.87.113.100/Renacer', 'http://www.1043hallelujahfm.com/', 'Hallelujah-1043.png', 0, 0, 0, 1, '2011-11-11', 13180, 0, '2017-01-01 00:00:00', 0), \
('Worship-Word-Warfare', '', 'mms://72.87.113.100/Renacer', '', 'Worship-Word-Warfare.png', 0, 0, 0, 1, '2011-11-11', 13181, 0, '2017-01-01 00:00:00', 0), \
('Good-Time-Oldies', '', 'mms://72.87.113.100/Renacer', '', 'Good-Time-Oldies.png', 0, 0, 0, 1, '2011-11-11', 13182, 0, '2017-01-01 00:00:00', 0), \
('1046-FM-RTL-Best-of-Black', 'The Best of Black &quot;gives you the highlights from Hip-Hop, R &amp; B and Soul direct and unfiltered as a web exclusive!', 'mms://72.87.113.100/Renacer', 'http://104.6rtl.com/', '1046-FM-RTL-Best-of-Black.png', 0, 0, 0, 1, '2011-11-11', 13183, 0, '2017-01-01 00:00:00', 0), \
('Dance90s', '', 'http://listen.radionomy.com/dance90', 'http://dance90.tk/', 'Dance90s.png', 0, 0, 0, 1, '2011-11-11', 13185, 0, '2017-01-01 00:00:00', 0), \
('WESC-FM-925', '', 'http://listen.radionomy.com/dance90', 'http://www.wescfm.com/', 'WESC-FM-925.png', 0, 0, 0, 1, '2011-11-11', 13186, 0, '2017-01-01 00:00:00', 0), \
('Peace-FM-1043', '', 'http://radio.ghanaweb.com/radio.asx?ID=1', 'http://www.peacefmonline.com/', 'Peace-FM-1043.png', 0, 0, 0, 1, '2011-11-11', 13187, 0, '2017-01-01 00:00:00', 0), \
('Fox-NFL-Rewind', '', 'http://radio.ghanaweb.com/radio.asx?ID=1', '', 'Fox-NFL-Rewind.png', 0, 0, 0, 1, '2011-11-11', 13188, 0, '2017-01-01 00:00:00', 0), \
('X1039', '', 'http://www.181.fm/winamp.pls?station=181-beatles&style=&description=Beatles', 'http://www.x1039.com/', 'X1039.png', 0, 0, 0, 1, '2011-11-11', 13189, 0, '2017-01-01 00:00:00', 0);";


const char *radio_station_setupsql41="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Clubbin-One-Radio', 'Dance actuelle, dance Golds 90&#39;s ! Le mix dancefloor music !', 'http://www.clubbinone.fr/listen.m3u', 'http://www.clubbinone.fr/', 'Clubbin-One-Radio.png', 0, 0, 0, 1, '2011-11-11', 13190, 0, '2017-01-01 00:00:00', 0), \
('KNON-893', 'KNON is a non-profit, listener-supported radio station, deriving its main source of income from on-air pledge drives and from underwriting or sponsorships by local small businesses.', 'http://www.warpradio.com/player/mediaserver.asp?id=9669&t=2&streamRate=', 'http://www.knon.org/', 'KNON-893.png', 0, 0, 0, 1, '2011-11-11', 13191, 0, '2017-01-01 00:00:00', 0), \
('The-Beat-1017', '', 'http://asx.abacast.com/longisland-wbeafm-64.pls', 'http://www.1017thebeat.com/', 'The-Beat-1017.png', 0, 0, 0, 1, '2011-11-11', 13192, 0, '2017-01-01 00:00:00', 0), \
('WNXT-1260', 'Today much of WNXT is dominated by ESPN Radio and local sports. The station dumped much of its country music in 2004 for ESPN Radio. The station has a major variety of both local and professional spor', 'http://asx.abacast.com/longisland-wbeafm-64.pls', 'http://www.angelfire.com/oh3/wnxt/wnxtam.html', 'WNXT-1260.png', 0, 0, 0, 1, '2011-11-11', 13193, 0, '2017-01-01 00:00:00', 0), \
('The-Voice-720', '', 'http://asx.abacast.com/longisland-wbeafm-64.pls', 'http://www.newsradio720.com/', 'The-Voice-720.png', 0, 0, 0, 1, '2011-11-11', 13194, 0, '2017-01-01 00:00:00', 0), \
('DIE-NEUE-1077', '', 'http://edge.live.mp3.mdn.newmedia.nacamar.net/ps-dieneue_rock/livestream_hi.mp3', 'http://www.dieneue1077.de/', 'DIE-NEUE-1077.png', 0, 0, 0, 1, '2011-11-11', 13195, 0, '2017-01-01 00:00:00', 0), \
('TapouT-Radio', 'The worlds #1 MMA radio show. We have the biggest names in MMA every Monday and Thursday at 9PM Eastern, and replays streaming 24/7.', 'http://smashedmedia.us/beta/tapout.m3u', 'http://www.tapoutlive.com', 'TapouT-Radio.png', 0, 0, 0, 1, '2011-11-11', 13196, 0, '2017-01-01 00:00:00', 0), \
('Z-969', '', 'http://smashedmedia.us/beta/tapout.m3u', 'http://www.kzbkradio.com/', 'Z-969.png', 0, 0, 0, 1, '2011-11-11', 13197, 0, '2017-01-01 00:00:00', 0), \
('WINZ-940', '', 'http://smashedmedia.us/beta/tapout.m3u', 'http://www.940winz.com', 'WINZ-940.png', 0, 0, 0, 1, '2011-11-11', 13198, 0, '2017-01-01 00:00:00', 0), \
('SNR-Postgame', '', 'http://smashedmedia.us/beta/tapout.m3u', '', 'SNR-Postgame.png', 0, 0, 0, 1, '2011-11-11', 13199, 0, '2017-01-01 00:00:00', 0), \
('Ill-Nio', '', 'http://smashedmedia.us/beta/tapout.m3u', '', 'Ill.png', 0, 0, 0, 1, '2011-11-11', 13200, 0, '2017-01-01 00:00:00', 0), \
('KBIG-979', 'KBIG 97.9 Hawaii and 106.1 Kona are Piped from the Big Island of Hawaii!', 'http://smashedmedia.us/beta/tapout.m3u', 'http://www.kbigfm.com/', 'KBIG-979.png', 0, 0, 0, 1, '2011-11-11', 13201, 0, '2017-01-01 00:00:00', 0), \
('Vancouver-Canucks-Play-by-Play', '', 'http://smashedmedia.us/beta/tapout.m3u', '', 'Vancouver-Canucks-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 13202, 0, '2017-01-01 00:00:00', 0), \
('House-Nation-UK', 'HouseNationUK Radio broadcasts live House Music 24 hours a day!', 'http://www.housenationuk.com:9000', 'http://housenationuk.com/', 'House-Nation-UK.png', 0, 0, 0, 1, '2011-11-11', 13203, 0, '2017-01-01 00:00:00', 0), \
('David-Gray', '', 'http://www.housenationuk.com:9000', '', 'David-Gray.png', 0, 0, 0, 1, '2011-11-11', 13204, 0, '2017-01-01 00:00:00', 0), \
('KDAA-1031', '', 'http://www.housenationuk.com:9000', 'http://www.resultsradioonline.com/', 'KDAA-1031.png', 0, 0, 0, 1, '2011-11-11', 13205, 0, '2017-01-01 00:00:00', 0), \
('Linas-Jazz', '', 'http://www.streamakaci.com/radios/linas.m3u', 'http://www.linasjazz.net/', 'Linas-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13206, 0, '2017-01-01 00:00:00', 0), \
('FM-100-1003', '', 'http://wmc1.den.liquidcompass.net/KSFIFM', 'http://www.fm100.com/', 'FM-100-1003.png', 0, 0, 0, 1, '2011-11-11', 13207, 0, '2017-01-01 00:00:00', 0), \
('Inside-Alabama-Racing', '', 'http://www.181.fm/winamp.pls?station=181-energy98&style=mp3&description=Energy%2098', '', 'Inside-Alabama-Racing.png', 0, 0, 0, 1, '2011-11-11', 13209, 0, '2017-01-01 00:00:00', 0), \
('Gas-a-Fondo', '', 'http://www.181.fm/winamp.pls?station=181-energy98&style=mp3&description=Energy%2098', '', 'Gas-a-Fondo.png', 0, 0, 0, 1, '2011-11-11', 13210, 0, '2017-01-01 00:00:00', 0), \
('Gospel-Experience', '', 'http://www.181.fm/winamp.pls?station=181-energy98&style=mp3&description=Energy%2098', '', 'Gospel-Experience.png', 0, 0, 0, 1, '2011-11-11', 13211, 0, '2017-01-01 00:00:00', 0), \
('Answering-The-Call', '', 'http://www.181.fm/winamp.pls?station=181-energy98&style=mp3&description=Energy%2098', '', 'Answering-The-Call.png', 0, 0, 0, 1, '2011-11-11', 13212, 0, '2017-01-01 00:00:00', 0), \
('Generation-X', '', 'http://www.181.fm/winamp.pls?station=181-energy98&style=mp3&description=Energy%2098', '', 'Generation-X.png', 0, 0, 0, 1, '2011-11-11', 13213, 0, '2017-01-01 00:00:00', 0), \
('1CLUBFM--Dance-Hits-Chicago', 'Today&#39;s dance hits live from Chicago: Visit our website for videos and more.', 'http://1club.fm/Listen/AAC/10010.pls', 'http://www.1club.fm/', '1CLUBFM--Dance-Hits-Chicago.png', 0, 0, 1, 1, '2011-11-11', 13214, 0, '2011-10-17 23:39:49', 0), \
('GotRadio-RB-Classics', 'Get down with the classic R&amp;B and soul sounds from the days of Motown and beyond.', 'http://www.gotradio.com/itunes/RnBClassics.pls', 'http://www.gotradio.com/', 'GotRadio-RB-Classics.png', 0, 0, 0, 1, '2011-11-11', 13215, 0, '2017-01-01 00:00:00', 0), \
('Paul-Jones', '', 'http://www.gotradio.com/itunes/RnBClassics.pls', '', 'Paul-Jones.png', 0, 0, 0, 1, '2011-11-11', 13216, 0, '2017-01-01 00:00:00', 0), \
('Rock-935', '', 'http://www.gotradio.com/itunes/RnBClassics.pls', 'http://www.warq.com/', 'Rock-935.png', 0, 0, 0, 1, '2011-11-11', 13217, 0, '2017-01-01 00:00:00', 0), \
('Country-Legends-1039', 'Legendary country music, concerts, everything Louisville', 'http://www.gotradio.com/itunes/RnBClassics.pls', 'http://countrylegends1039.com/', 'Country-Legends-1039.png', 0, 0, 0, 1, '2011-11-11', 13218, 0, '2017-01-01 00:00:00', 0), \
('KPIG-FM-1075', '', 'http://www.gotradio.com/itunes/RnBClassics.pls', 'http://www.kpig.com/', 'KPIG-FM-1075.png', 0, 0, 0, 1, '2011-11-11', 13219, 0, '2017-01-01 00:00:00', 0), \
('Albert-King', '', 'http://www.gotradio.com/itunes/RnBClassics.pls', '', 'Albert-King.png', 0, 0, 1, 1, '2011-11-11', 13220, 0, '2011-10-14 15:59:29', 0), \
('Triple-M-1051', '', 'http://sc.mel.triplem.com.au/listen.pls', 'http://www.triplem.com.au/melbourne', 'Triple-M-1051.png', 0, 0, 0, 1, '2011-11-11', 13221, 0, '2017-01-01 00:00:00', 0), \
('WIND-560', '', 'http://sc.mel.triplem.com.au/listen.pls', 'http://www.560wind.com/', 'WIND-560.png', 0, 0, 0, 1, '2011-11-11', 13222, 0, '2017-01-01 00:00:00', 0), \
('Thunder-106-1063', '', 'http://sc.mel.triplem.com.au/listen.pls', 'http://www.thunder106.com/', 'Thunder-106-1063.png', 0, 0, 0, 1, '2011-11-11', 13224, 0, '2017-01-01 00:00:00', 0), \
('Faith-Memorial-Church', '', 'http://sc.mel.triplem.com.au/listen.pls', '', 'Faith-Memorial-Church.png', 0, 0, 0, 1, '2011-11-11', 13225, 0, '2017-01-01 00:00:00', 0), \
('Classic-Jenny-FM', '', 'http://85.214.52.25:8500/', 'http://classic.jenny.fm/', 'Classic-Jenny-FM.png', 0, 0, 0, 1, '2011-11-11', 13226, 0, '2017-01-01 00:00:00', 0), \
('Dallas-Stars-at-Anaheim-Ducks-Oct-21-2011', '', 'http://85.214.52.25:8500/', '', '', 0, 0, 0, 1, '2011-11-11', 13227, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-Memphis-730', '', 'http://85.214.52.25:8500/', 'http://www.730foxsports.com/', 'Fox-Sports-Memphis-730.png', 0, 0, 0, 1, '2011-11-11', 13228, 0, '2017-01-01 00:00:00', 0), \
('KZIM-960', '', 'http://icecast.stretchinternet.com:8000/kzim', 'http://www.960kzim.com/', 'KZIM-960.png', 0, 0, 0, 1, '2011-11-11', 13229, 0, '2017-01-01 00:00:00', 0), \
('Salt-and-Light-Catholic-Radio-1140', '', 'http://salt-light-stream-01.miriamtech.net:8000/live', 'http://www.saltandlightradio.com/', 'Salt-and-Light-Catholic-Radio-1140.png', 0, 0, 0, 1, '2011-11-11', 13230, 0, '2017-01-01 00:00:00', 0), \
('WCOA-1370', '', 'mms://live.cumulusstreaming.com/WCOA-AM', 'http://www.wcoapensacola.com/', 'WCOA-1370.png', 0, 0, 0, 1, '2011-11-11', 13231, 0, '2017-01-01 00:00:00', 0), \
('WMAY-970', '', 'mms://live.cumulusstreaming.com/WCOA-AM', 'http://www.wmay.com/', 'WMAY-970.png', 0, 0, 0, 1, '2011-11-11', 13232, 0, '2017-01-01 00:00:00', 0), \
('Morning-Radio', '', 'mms://live.cumulusstreaming.com/WCOA-AM', '', 'Morning-Radio.png', 0, 0, 0, 1, '2011-11-11', 13233, 0, '2017-01-01 00:00:00', 0), \
('Young-Jeezy', '', 'mms://live.cumulusstreaming.com/WCOA-AM', '', 'Young-Jeezy.png', 0, 0, 0, 1, '2011-11-11', 13234, 0, '2017-01-01 00:00:00', 0), \
('4KQ-693', '', 'http://icecast.arn.com.au/4kq.aac.m3u', 'http://www.4kq.com.au/', '4KQ-693.png', 0, 0, 0, 1, '2011-11-11', 13235, 0, '2017-01-01 00:00:00', 0), \
('Motley-Fool-Money-Radio-Show', '', 'http://icecast.arn.com.au/4kq.aac.m3u', '', 'Motley-Fool-Money-Radio-Show.png', 0, 0, 0, 1, '2011-11-11', 13236, 0, '2017-01-01 00:00:00', 0), \
('WWJ-Newsradio-950', 'WWJ Newsradio 950 is Detroit&#39;s award winning all-news radio station and is live, local and committed to providing metro Detroiters the place to turn for up-to-the-minute news, traffic, sports, bus', 'http://icecast.arn.com.au/4kq.aac.m3u', 'http://www.wwj.com/', 'WWJ-Newsradio-950.png', 0, 0, 0, 1, '2011-11-11', 13237, 0, '2017-01-01 00:00:00', 0), \
('Rajawali-Radio', '', 'http://125.160.17.21:8012', 'http://rajawaliradio.com/', 'Rajawali-Radio.png', 0, 0, 0, 1, '2011-11-11', 13238, 0, '2017-01-01 00:00:00', 0), \
('Ray-LaMontagne', '', 'http://125.160.17.21:8012', '', 'Ray-LaMontagne.png', 0, 0, 0, 1, '2011-11-11', 13239, 0, '2017-01-01 00:00:00', 0), \
('965-KLR-965', '', 'http://125.160.17.21:8012', 'http://965klr.com/', '965-KLR-965.png', 0, 0, 0, 1, '2011-11-11', 13240, 0, '2017-01-01 00:00:00', 0), \
('101-5-Bob-Rocks-1015', '', 'http://125.160.17.21:8012', 'http://www.1015bobrocks.com/', '101-5-Bob-Rocks-1015.png', 0, 0, 0, 1, '2011-11-11', 13241, 0, '2017-01-01 00:00:00', 0), \
('ClubZonefm', '', 'http://s7.viastreaming.net:7420/', 'http://www.clubzone.fm/', 'ClubZonefm.png', 0, 0, 0, 1, '2011-11-11', 13242, 0, '2017-01-01 00:00:00', 0), \
('Pro-Wrestling-Report', '', 'http://s7.viastreaming.net:7420/', '', 'Pro-Wrestling-Report.png', 0, 0, 0, 1, '2011-11-11', 13243, 0, '2017-01-01 00:00:00', 0), \
('River-Country-1075', '', 'http://s7.viastreaming.net:7420/', 'http://www.wnntfm.com/', 'River-Country-1075.png', 0, 0, 0, 1, '2011-11-11', 13244, 0, '2017-01-01 00:00:00', 0), \
('Talkin-Baseball', '', 'http://s7.viastreaming.net:7420/', '', 'Talkin-Baseball.png', 0, 0, 0, 1, '2011-11-11', 13245, 0, '2017-01-01 00:00:00', 0), \
('All-Irish-Radio', '', 'http://173.244.194.212:8051/', 'http://www.allirishradio.com/', 'All-Irish-Radio.png', 0, 0, 1, 1, '2011-11-11', 13246, 0, '2011-10-14 15:59:33', 0), \
('GotRadio-Indie-Underground', 'The best of the best of undiscovered, untapped and unsigned rock.  Discover what&#39;s coming out of your neighborhood garages!', 'http://www.gotradio.com/itunes/Indie_Underground.pls', 'http://www.gotradio.com/', 'GotRadio-Indie-Underground.png', 0, 0, 0, 1, '2011-11-11', 13247, 0, '2017-01-01 00:00:00', 0), \
('Ceonta', '', 'http://www.gotradio.com/itunes/Indie_Underground.pls', '', 'Ceonta.png', 0, 0, 0, 1, '2011-11-11', 13248, 0, '2017-01-01 00:00:00', 0), \
('Dorothy-Ashby', '', 'http://www.live365.com/cgi-bin/wmp.asx?name_type=nam&station=jimiking', '', 'Dorothy-Ashby.png', 0, 0, 0, 1, '2011-11-11', 13250, 0, '2017-01-01 00:00:00', 0), \
('The-Academy-Is', '', 'http://www.live365.com/cgi-bin/wmp.asx?name_type=nam&station=jimiking', '', 'The-Academy-Is.png', 0, 0, 0, 1, '2011-11-11', 13251, 0, '2017-01-01 00:00:00', 0), \
('Big-1290', '', 'http://www.live365.com/cgi-bin/wmp.asx?name_type=nam&station=jimiking', 'http://www.wfbg.com/', 'Big-1290.png', 0, 0, 0, 1, '2011-11-11', 13252, 0, '2017-01-01 00:00:00', 0), \
('KLBJ-FM-937', '', 'http://www.live365.com/cgi-bin/wmp.asx?name_type=nam&station=jimiking', 'http://www.klbjfm.com/', 'KLBJ-FM-937.png', 0, 0, 0, 1, '2011-11-11', 13253, 0, '2017-01-01 00:00:00', 0), \
('WSM-FM-955', '', 'mms://live.cumulusstreaming.com/WSM-FM', 'http://www.955thewolf.com/', 'WSM-FM-955.png', 0, 0, 0, 1, '2011-11-11', 13254, 0, '2017-01-01 00:00:00', 0), \
('The-Marshall-Tucker-Band', '', 'mms://live.cumulusstreaming.com/WSM-FM', '', 'The-Marshall-Tucker-Band.png', 0, 0, 0, 1, '2011-11-11', 13255, 0, '2017-01-01 00:00:00', 0), \
('WWNR-620', '', 'http://72.167.37.48:8282/', 'http://www.wwnrnewstalk620.com/', 'WWNR-620.png', 0, 0, 0, 1, '2011-11-11', 13256, 0, '2017-01-01 00:00:00', 0), \
('At-the-Track', '', 'http://72.167.37.48:8282/', '', 'At-the-Track.png', 0, 0, 0, 1, '2011-11-11', 13257, 0, '2017-01-01 00:00:00', 0), \
('Underoath', '', 'http://72.167.37.48:8282/', '', 'Underoath.png', 0, 0, 0, 1, '2011-11-11', 13258, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Afternoon-Jazz-(CJRT)', '', 'http://72.167.37.48:8282/', '', 'Sunday-Afternoon-Jazz-(CJRT).png', 0, 0, 0, 1, '2011-11-11', 13259, 0, '2017-01-01 00:00:00', 0), \
('CountryManiaRadio', 'CountryManiaRadio is a radio who&#39;s listening to there listerners. At all the time the listerners can make request even in the non-stop', 'http://207.210.80.170:9030/', 'http://www.countrymaniaradio.com/', 'CountryManiaRadio.png', 0, 0, 0, 1, '2011-11-11', 13261, 0, '2017-01-01 00:00:00', 0), \
('WGSO-990', 'WGSO 990 is a news, talk, and sports AM radio station based in New Orleans, Louisiana.', 'http://win1.san.fast-serv.com/wms916126', 'http://wgso.com/', 'WGSO-990.png', 0, 0, 0, 1, '2011-11-11', 13262, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1500', '', 'mms://live.cumulusstreaming.com/WAYS-AM', 'http://www.waysam.com/', 'The-Fan-1500.png', 0, 0, 0, 1, '2011-11-11', 13263, 0, '2017-01-01 00:00:00', 0), \
('Jelly-Radio', 'JellyRadio.com, an Internet music source for hip-hop and RB', 'http://74.123.96.30:8022/', 'http://www.jellyradio.com/', 'Jelly-Radio.png', 0, 0, 0, 1, '2011-11-11', 13264, 0, '2017-01-01 00:00:00', 0), \
('Radio-Bop', 'Radio Bop plays 50s and 60s Rock Roll from early Rockabilly through early Beatles with a playlist based on the Billboard Top 40', 'http://208.53.158.48:8362', 'http://www.radiobop.com/', 'Radio-Bop.png', 0, 0, 0, 1, '2011-11-11', 13265, 0, '2017-01-01 00:00:00', 0), \
('Defjay-Radio', '100% RB Radio! The place to be in RB!', 'http://listen.defjay.com:80/listen.pls', 'http://www.defjay.com/', 'Defjay-Radio.png', 0, 0, 0, 1, '2011-11-11', 13266, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Heavy-Hitting-Boxing-Podcast', '', 'http://listen.defjay.com:80/listen.pls', '', 'ESPN-Heavy-Hitting-Boxing-Podcast.png', 0, 0, 0, 1, '2011-11-11', 13267, 0, '2017-01-01 00:00:00', 0), \
('WLRN-FM-913', '', 'http://204.13.4.157:8000', 'http://www.wlrn.org/', 'WLRN-FM-913.png', 0, 0, 0, 1, '2011-11-11', 13268, 0, '2017-01-01 00:00:00', 0), \
('WLQY-1320', '', 'http://s5.voscast.com:7382', 'http://www.doucefm.com', 'WLQY-1320.png', 0, 0, 0, 1, '2011-11-11', 13269, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1330', '', 'mms://69.7.99.200:2001/', 'http://www.sportsradio1330.com/', 'The-Fan-1330.png', 0, 0, 0, 1, '2011-11-11', 13270, 0, '2017-01-01 00:00:00', 0), \
('WCMF-FM-965', '', 'mms://69.7.99.200:2001/', 'http://www.wcmf.com/', 'WCMF-FM-965.png', 0, 0, 0, 1, '2011-11-11', 13271, 0, '2017-01-01 00:00:00', 0), \
('WRMT-1490', '', 'http://tqsoulparty.primcast.com:9378', '', 'WRMT-1490.png', 0, 0, 0, 1, '2011-11-11', 13272, 0, '2017-01-01 00:00:00', 0), \
('John-Fogerty', '', 'http://tqsoulparty.primcast.com:9378', '', 'John-Fogerty.png', 0, 0, 0, 1, '2011-11-11', 13274, 0, '2017-01-01 00:00:00', 0), \
('WRXB-1590', '', 'http://tqsoulparty.primcast.com:9378', 'http://www.wrxb.us/', 'WRXB-1590.png', 0, 0, 0, 1, '2011-11-11', 13275, 0, '2017-01-01 00:00:00', 0), \
('Eurodance-90-Radio', '', 'http://listen.radionomy.com/eurodance-90', 'http://eurodance90.fr/', 'Eurodance-90-Radio.png', 0, 0, 0, 1, '2011-11-11', 13276, 0, '2017-01-01 00:00:00', 0), \
('Lite-Rock-973', '', 'http://listen.radionomy.com/eurodance-90', 'http://www.literock973.com/', 'Lite-Rock-973.png', 0, 0, 0, 1, '2011-11-11', 13277, 0, '2017-01-01 00:00:00', 0), \
('Mix-929', '', 'http://listen.radionomy.com/eurodance-90', 'http://www.mymix929.com/', 'Mix-929.png', 0, 0, 0, 1, '2011-11-11', 13278, 0, '2017-01-01 00:00:00', 0), \
('NCB-Radio', 'NCB Radio was founded in March 2010 by a group of young people in North Cornwall with one single aim- to create a radio station that is more than just a radio station. It is our mission to use the med', 'http://uk3-vn.mixstream.net:8244/', 'http://www.ncbradio.co.uk/', 'NCB-Radio.png', 0, 0, 0, 1, '2011-11-11', 13279, 0, '2017-01-01 00:00:00', 0), \
('Alanis-Morissette', '', 'http://uk3-vn.mixstream.net:8244/', '', 'Alanis-Morissette.png', 0, 0, 1, 1, '2011-11-11', 13280, 0, '2011-10-14 15:59:23', 0), \
('After-7', '', 'http://uk3-vn.mixstream.net:8244/', '', 'After-7.png', 0, 0, 0, 1, '2011-11-11', 13281, 0, '2017-01-01 00:00:00', 0), \
('Ra', '', 'http://uk3-vn.mixstream.net:8244/', '', 'Ra.png', 0, 0, 0, 1, '2011-11-11', 13282, 0, '2017-01-01 00:00:00', 0), \
('3-Doors-Down', '', 'http://uk3-vn.mixstream.net:8244/', '', '3-Doors-Down.png', 0, 0, 0, 1, '2011-11-11', 13283, 0, '2017-01-01 00:00:00', 0), \
('WHGL-FM-1003', '', 'http://24.229.175.57:88/broadwave.m3u?src=2&rate=1', 'http://www.wiggle100.com/', 'WHGL-FM-1003.png', 0, 0, 0, 1, '2011-11-11', 13284, 0, '2017-01-01 00:00:00', 0), \
('KHUB-1340', '', 'mms://streamer.omni-tech.net/KHUB', 'http://www.khubradio.com/', 'KHUB-1340.png', 0, 0, 0, 1, '2011-11-11', 13285, 0, '2017-01-01 00:00:00', 0), \
('4KIG-1071', '', 'mms://streamer.omni-tech.net/KHUB', 'http://www.4k1g.org/', '4KIG-1071.png', 0, 0, 0, 1, '2011-11-11', 13286, 0, '2017-01-01 00:00:00', 0), \
('Sunny-1043', 'Sunny 104.3 has been South Florida&#39;s number one choice for music variety for over 15 years! From the morning show with Rick and Michelle and right into the night with Delilah', 'http://sc12.1.fm:7078', 'http://www.sunny1043.com/', 'Sunny-1043.png', 0, 0, 0, 1, '2011-11-11', 13289, 0, '2017-01-01 00:00:00', 0), \
('Steppin-Out', '', 'http://sc12.1.fm:7078', '', 'Steppin-Out.png', 0, 0, 0, 1, '2011-11-11', 13290, 0, '2017-01-01 00:00:00', 0), \
('Your-Money-Matters', 'Your Money Matters! promises to deliver useful information in an entertaining format that will help you in your everyday financial lives.', 'http://www.live365.com/play/yourmoneymatters?tag=itunes&sid=24.148.45.243-1294171599781609&lid=602-usa&from=pls', 'http://www.yourmoneymattersradio.com/', 'Your-Money-Matters.png', 0, 0, 0, 1, '2011-11-11', 13291, 0, '2017-01-01 00:00:00', 0), \
('KGLO-1300', '', 'http://icy2.abacast.com/threeeagles-mcwkglofm-32.m3u', 'http://www.kgloam.com/', 'KGLO-1300.png', 0, 0, 0, 1, '2011-11-11', 13292, 0, '2017-01-01 00:00:00', 0), \
('KDQN-1390', '', 'http://icy2.abacast.com/threeeagles-mcwkglofm-32.m3u', 'http://www.kdqn.net/', 'KDQN-1390.png', 0, 0, 0, 1, '2011-11-11', 13293, 0, '2017-01-01 00:00:00', 0), \
('Cassius', '', 'http://icy2.abacast.com/threeeagles-mcwkglofm-32.m3u', '', 'Cassius.png', 0, 0, 0, 1, '2011-11-11', 13294, 0, '2017-01-01 00:00:00', 0), \
('The-Mustard--Johnson-Show', '', 'http://icy2.abacast.com/threeeagles-mcwkglofm-32.m3u', '', 'The-Mustard--Johnson-Show.png', 0, 0, 0, 1, '2011-11-11', 13295, 0, '2017-01-01 00:00:00', 0), \
('Puma-Radio', '', 'http://85.25.149.109:7000/', 'http://www.radio-puma.de/', 'Puma-Radio.png', 0, 0, 0, 1, '2011-11-11', 13296, 0, '2017-01-01 00:00:00', 0), \
('Thelma-Houston', '', 'http://abc.net.au/res/streaming/audio/mp3/classic_fm.pls', '', 'Thelma-Houston.png', 0, 0, 0, 1, '2011-11-11', 13298, 0, '2017-01-01 00:00:00', 0), \
('Joe-Jackson', '', 'http://abc.net.au/res/streaming/audio/mp3/classic_fm.pls', '', 'Joe-Jackson.png', 0, 0, 0, 1, '2011-11-11', 13299, 0, '2017-01-01 00:00:00', 0), \
('Swing--Big-Band-on-JAZZRADIOcom', 'The best sounds of the swing era of the 30s &amp; 40s.', 'http://listen.jazzradio.com/partner_aac/swingnbigband.pls', 'http://www.jazzradio.com/', 'Swing--Big-Band-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 13300, 0, '2017-01-01 00:00:00', 0), \
('Deutschlandradio-Kultur-896', 'Deutschlandradio Kultur ist umfassender H&#246;rgenuss - f&#252;r Menschen, denen Musik, Kulturinformationen und anregende Unterhaltung wichtig sind.', 'http://www.dradio.de/streaming/dkultur.m3u', 'http://www.dradio.de/dkultur/', 'Deutschlandradio-Kultur-896.png', 0, 0, 0, 1, '2011-11-11', 13301, 0, '2017-01-01 00:00:00', 0), \
('SKYFM-Alt-Rock', 'Alternative rock hits You want to hear!!', 'http://listen.sky.fm/partner_mp3/altrock.pls', 'http://www.sky.fm/altrock', 'SKYFM-Alt-Rock.png', 0, 0, 0, 1, '2011-11-11', 13302, 0, '2017-01-01 00:00:00', 0), \
('WLJA-1011', '', 'http://listen.sky.fm/partner_mp3/altrock.pls', 'http://www.wljaradio.com/', 'WLJA-1011.png', 0, 0, 0, 1, '2011-11-11', 13303, 0, '2017-01-01 00:00:00', 0), \
('Fidelity-957', '', 'http://live-icy.gss.dr.dk:8000/Channel3_HQ.mp3', 'http://www.fidelitypr.com/', 'Fidelity-957.png', 0, 0, 0, 1, '2011-11-11', 13304, 0, '2017-01-01 00:00:00', 0), \
('ESPN-690', '', 'http://live-icy.gss.dr.dk:8000/Channel3_HQ.mp3', '', 'ESPN-690.png', 0, 0, 0, 1, '2011-11-11', 13305, 0, '2017-01-01 00:00:00', 0), \
('The-Cardigans', '', 'http://live-icy.gss.dr.dk:8000/Channel3_HQ.mp3', '', 'The-Cardigans.png', 0, 0, 0, 1, '2011-11-11', 13306, 0, '2017-01-01 00:00:00', 0), \
('KISS-Country-999', '', 'http://live-icy.gss.dr.dk:8000/Channel3_HQ.mp3', 'http://www.wkis.com/', 'KISS-Country-999.png', 0, 0, 0, 1, '2011-11-11', 13307, 0, '2017-01-01 00:00:00', 0), \
('Sixties-Sunday-With-Dean-Martin', '', 'http://live-icy.gss.dr.dk:8000/Channel3_HQ.mp3', '', 'Sixties-Sunday-With-Dean-Martin.png', 0, 0, 0, 1, '2011-11-11', 13308, 0, '2017-01-01 00:00:00', 0), \
('WPLN-1430', 'NPR Talk and World News 24 Hours a Day', 'http://wpln.streamguys.org/wplnam.mp3.m3u', 'http://wpln.org/', 'WPLN-1430.png', 0, 0, 0, 1, '2011-11-11', 13309, 0, '2017-01-01 00:00:00', 0), \
('K-Shore-973', '', 'http://wpln.streamguys.org/wplnam.mp3.m3u', 'http://www.kshr.com/', 'K-Shore-973.png', 0, 0, 0, 1, '2011-11-11', 13310, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-995', '', 'http://wpln.streamguys.org/wplnam.mp3.m3u', 'http://www.thewolfonline.com/', 'The-Wolf-995.png', 0, 0, 0, 1, '2011-11-11', 13311, 0, '2017-01-01 00:00:00', 0), \
('Passport-Approved', '', 'http://shoutatme.com:17240/listen.pls', '', 'Passport-Approved.png', 0, 0, 0, 1, '2011-11-11', 13313, 0, '2017-01-01 00:00:00', 0), \
('Natasha-Bedingfield', '', 'http://shoutatme.com:17240/listen.pls', '', 'Natasha-Bedingfield.png', 0, 0, 0, 1, '2011-11-11', 13314, 0, '2017-01-01 00:00:00', 0), \
('WSEI-929', '', 'http://shoutatme.com:17240/listen.pls', 'http://www.929thelegend.com/', 'WSEI-929.png', 0, 0, 0, 1, '2011-11-11', 13315, 0, '2017-01-01 00:00:00', 0), \
('KMAJ-1440', '', 'mms://live.cumulusstreaming.com/KMAJ-AM', 'http://kmaj1440.com/', 'KMAJ-1440.png', 0, 0, 0, 1, '2011-11-11', 13317, 0, '2017-01-01 00:00:00', 0), \
('Juanes', '', 'mms://live.cumulusstreaming.com/KMAJ-AM', '', 'Juanes.png', 0, 0, 0, 1, '2011-11-11', 13318, 0, '2017-01-01 00:00:00', 0), \
('Boystown-Live-Dance-Radio', '', 'http://boystownlive.com:6161/', 'http://boystownlive.com/', 'Boystown-Live-Dance-Radio.png', 0, 0, 0, 1, '2011-11-11', 13319, 0, '2017-01-01 00:00:00', 0), \
('KFOG-1045', '', 'mms://live.cumulusstreaming.com/KFOG-FM', 'http://www.kfog.com/', 'KFOG-1045.png', 0, 0, 0, 1, '2011-11-11', 13321, 0, '2017-01-01 00:00:00', 0), \
('Nashville-Classics', '', 'http://174.120.124.178:7110/', 'http://www.nashvilleclassics.com/', 'Nashville-Classics.png', 0, 0, 0, 1, '2011-11-11', 13322, 0, '2017-01-01 00:00:00', 0), \
('Agoria', '', 'http://174.120.124.178:7110/', '', 'Agoria.png', 0, 0, 0, 1, '2011-11-11', 13323, 0, '2017-01-01 00:00:00', 0), \
('Froggy-949', '', 'http://174.120.124.178:7110/', 'http://www.froggyland.com/index.php?site=949_940', 'Froggy-949.png', 0, 0, 0, 1, '2011-11-11', 13324, 0, '2017-01-01 00:00:00', 0), \
('Georgia-Football-Pre-Game', '', 'http://174.120.124.178:7110/', '', 'Georgia-Football-Pre-Game.png', 0, 0, 0, 1, '2011-11-11', 13325, 0, '2017-01-01 00:00:00', 0), \
('Italian-Graffiati', 'In diretta live da Montr&#233;al (Qu&#233;bec)<BR>Canada, per tutti gli italiani nel mondo trasmettiamo le piu&#39; belle selezioni musicali dei favolosi anni  &#39;60 e &#39;70.<BR>La grande musica i', 'http://ibiza.broadstreamer.com:8000', 'http://www.italiangraffiati.com/', 'Italian-Graffiati.png', 0, 0, 0, 1, '2011-11-11', 13327, 0, '2017-01-01 00:00:00', 0), \
('KNOW-FM-911', '', 'http://minnesota.publicradio.org/tools/play/streams/news.pls', 'http://minnesota.publicradio.org/features/', 'KNOW-FM-911.png', 0, 0, 0, 1, '2011-11-11', 13328, 0, '2017-01-01 00:00:00', 0), \
('The-Home-Team', '', 'http://minnesota.publicradio.org/tools/play/streams/news.pls', '', 'The-Home-Team.png', 0, 0, 0, 1, '2011-11-11', 13329, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-1050', '', 'http://stream2.securenetsystems.net/WDZA', 'http://www.espndecatur.com/', 'The-Fan-1050.png', 0, 0, 0, 1, '2011-11-11', 13330, 0, '2017-01-01 00:00:00', 0), \
('Phillies-247-981', '', 'http://stream2.securenetsystems.net/WDZA', 'http://www.wogl.com/', 'Phillies-247-981.png', 0, 0, 0, 1, '2011-11-11', 13331, 0, '2017-01-01 00:00:00', 0), \
('Authentic-Sht', '', 'http://stream2.securenetsystems.net/WDZA', '', 'Authentic-Sht.png', 0, 0, 0, 1, '2011-11-11', 13332, 0, '2017-01-01 00:00:00', 0), \
('Free', '', 'http://stream2.securenetsystems.net/WDZA', '', 'Free.png', 0, 0, 0, 1, '2011-11-11', 13333, 0, '2017-01-01 00:00:00', 0), \
('Grassy-Hill-Radio', '', 'http://radio.grassyhill.org/listen.pls', '', 'Grassy-Hill-Radio.png', 0, 0, 0, 1, '2011-11-11', 13336, 0, '2017-01-01 00:00:00', 0), \
('KXRO-1320', 'With over 77 years of community service, we are Grays Harbor Radio 1320 KXRO AM Aberdeen WA', 'http://radio.grassyhill.org/listen.pls', 'http://www.kxro.com/', 'KXRO-1320.png', 0, 0, 0, 1, '2011-11-11', 13337, 0, '2017-01-01 00:00:00', 0), \
('The-ACE--TJ-Show', '', 'http://radio.grassyhill.org/listen.pls', '', 'The-ACE--TJ-Show.png', 0, 0, 0, 1, '2011-11-11', 13338, 0, '2017-01-01 00:00:00', 0), \
('La-Quinta-Estacin', '', 'http://radio.grassyhill.org/listen.pls', '', 'La-Quinta-Estacin.png', 0, 0, 0, 1, '2011-11-11', 13339, 0, '2017-01-01 00:00:00', 0), \
('KVSL-1450', '', 'http://radio.grassyhill.org/listen.pls', 'http://www.whitemountainradio.com/', 'KVSL-1450.png', 0, 0, 0, 1, '2011-11-11', 13340, 0, '2017-01-01 00:00:00', 0), \
('The-Outfield', '', 'http://radio.grassyhill.org/listen.pls', '', 'The-Outfield.png', 0, 0, 0, 1, '2011-11-11', 13341, 0, '2017-01-01 00:00:00', 0), \
('Moist', '', 'http://radio.grassyhill.org/listen.pls', '', 'Moist.png', 0, 0, 0, 1, '2011-11-11', 13342, 0, '2017-01-01 00:00:00', 0), \
('Afternoon-Jazz-with-Wendy-Fopeano', '', 'http://radio.grassyhill.org/listen.pls', '', 'Afternoon-Jazz-with-Wendy-Fopeano.png', 0, 0, 0, 1, '2011-11-11', 13343, 0, '2017-01-01 00:00:00', 0), \
('1LIVE---Das-junge-Radio-des-WDR-1024', '1LIVE spielt und macht Hits. Mit aktuellen Nachrichten, Informationen sowie unterhaltenden Beitr&#228;gen 24 Stunden am Tag.', 'http://www.wdr.de/wdrlive/media/einslive.m3u', 'http://www.einslive.de/', '1LIVE---Das-junge-Radio-des-WDR-1024.png', 0, 0, 0, 1, '2011-11-11', 13344, 0, '2017-01-01 00:00:00', 0), \
('French-Affair', '', 'http://www.wdr.de/wdrlive/media/einslive.m3u', '', 'French-Affair.png', 0, 0, 0, 1, '2011-11-11', 13345, 0, '2017-01-01 00:00:00', 0), \
('Talking-Heads', '', 'http://www.wdr.de/wdrlive/media/einslive.m3u', '', 'Talking-Heads.png', 0, 0, 0, 1, '2011-11-11', 13346, 0, '2017-01-01 00:00:00', 0), \
('The-River-975', '', 'http://www.wdr.de/wdrlive/media/einslive.m3u', 'http://www.ckrv.com/', 'The-River-975.png', 0, 0, 0, 1, '2011-11-11', 13347, 0, '2017-01-01 00:00:00', 0), \
('Paradise-Lost', '', 'http://www.wdr.de/wdrlive/media/einslive.m3u', '', 'Paradise-Lost.png', 0, 0, 0, 1, '2011-11-11', 13348, 0, '2017-01-01 00:00:00', 0), \
('Swedish-House-Mafia', '', 'http://www.wdr.de/wdrlive/media/einslive.m3u', '', 'Swedish-House-Mafia.png', 0, 0, 0, 1, '2011-11-11', 13349, 0, '2017-01-01 00:00:00', 0), \
('Mix-1011', '', 'http://icecast.arn.com.au/1011.aac.m3u', 'http://www.mix1011.com.au/', 'Mix-1011.png', 0, 0, 0, 1, '2011-11-11', 13350, 0, '2017-01-01 00:00:00', 0), \
('Star-1033', '', 'http://icecast.arn.com.au/1011.aac.m3u', 'http://star103fm.com/', 'Star-1033.png', 0, 0, 0, 1, '2011-11-11', 13351, 0, '2017-01-01 00:00:00', 0), \
('Sunset-Rubdown', '', 'http://icecast.arn.com.au/1011.aac.m3u', '', 'Sunset-Rubdown.png', 0, 0, 0, 1, '2011-11-11', 13352, 0, '2017-01-01 00:00:00', 0), \
('my-999', '', 'http://icecast.arn.com.au/1011.aac.m3u', 'http://www.my999radio.com/', 'my-999.png', 0, 0, 0, 1, '2011-11-11', 13353, 0, '2017-01-01 00:00:00', 0), \
('Above-Top-Secret-Radio', '', 'http://69.175.39.146:8650/', 'http://www.abovetopsecret.com/', 'Above-Top-Secret-Radio.png', 0, 0, 0, 1, '2011-11-11', 13354, 0, '2017-01-01 00:00:00', 0), \
('The-Band', '', 'http://69.175.39.146:8650/', '', 'The-Band.png', 0, 0, 0, 1, '2011-11-11', 13355, 0, '2017-01-01 00:00:00', 0), \
('Rock-979-NRQ', '', 'mms://live.cumulusstreaming.com/KNRQ-FM', 'http://www.nrq.com/', 'Rock-979-NRQ.png', 0, 0, 0, 1, '2011-11-11', 13356, 0, '2017-01-01 00:00:00', 0), \
('FM-Uruguay-879', 'Radio Uruguay, A Partir Del Dia 24 De Octubre De 2006 Deja La Frecuencia 90.9 Mhz Despues De 14', 'http://200.58.116.222:11130/', 'http://www.fmuruguay.com.ar/', 'FM-Uruguay-879.png', 0, 0, 0, 1, '2011-11-11', 13357, 0, '2017-01-01 00:00:00', 0), \
('SomaFM-Secret-Agent', 'Only from SomaFM.com.', 'http://api.somafm.com/secretagent130.pls', 'http://somafm.com/play/secretagent', 'SomaFM-Secret-Agent.png', 0, 0, 0, 1, '2011-11-11', 13358, 0, '2017-01-01 00:00:00', 0), \
('WLLM-1370', 'WLLM AM1370 Easy Listening Christian Radio in Lincoln, IL and Central Illinois.  Looking for Christian Radio Stations in Central Illinois', 'http://api.somafm.com/secretagent130.pls', 'http://www.wllmradio.com/', 'WLLM-1370.png', 0, 0, 0, 1, '2011-11-11', 13359, 0, '2017-01-01 00:00:00', 0), \
('Health-Talk', '', 'http://api.somafm.com/secretagent130.pls', '', 'Health-Talk.png', 0, 0, 0, 1, '2011-11-11', 13360, 0, '2017-01-01 00:00:00', 0), \
('Medina', '', 'http://api.somafm.com/secretagent130.pls', '', 'Medina.png', 0, 0, 0, 1, '2011-11-11', 13361, 0, '2017-01-01 00:00:00', 0), \
('Kiss-1053', '', 'http://ice3.securenetsystems.net:80/WYKSM', 'http://www.kiss1053.com/', 'Kiss-1053.png', 0, 0, 0, 1, '2011-11-11', 13362, 0, '2017-01-01 00:00:00', 0), \
('Exa-FM-975', '', 'http://www.grupoalius.com/exa.m3u', 'http://www.exafm.com.mx/guatemala', 'Exa-FM-975.png', 0, 0, 0, 1, '2011-11-11', 13363, 0, '2017-01-01 00:00:00', 0), \
('NHL-Play-by-Play', 'The puck drops every day on the world&#39;s fastest sport on the world&#39;s first hockey channel.', 'http://www.grupoalius.com/exa.m3u', 'http://www.xmradio.com/', 'NHL-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 13365, 0, '2017-01-01 00:00:00', 0), \
('Post-Harbor', '', 'http://www.grupoalius.com/exa.m3u', '', 'Post-Harbor.png', 0, 0, 0, 1, '2011-11-11', 13366, 0, '2017-01-01 00:00:00', 0), \
('Radio-Polskie---Soul', '', 'http://91.121.77.187:6300', 'http://www.polskastacja.pl/', 'Radio-Polskie---Soul.png', 0, 0, 0, 1, '2011-11-11', 13367, 0, '2017-01-01 00:00:00', 0), \
('KINN-1270', '', 'http://91.121.77.187:6300', '', 'KINN-1270.png', 0, 0, 0, 1, '2011-11-11', 13368, 0, '2017-01-01 00:00:00', 0), \
('The-Great-Outdoors', '', 'http://91.121.77.187:6300', '', 'The-Great-Outdoors.png', 0, 0, 0, 1, '2011-11-11', 13369, 0, '2017-01-01 00:00:00', 0), \
('Nihal', '', 'http://91.121.77.187:6300', '', 'Nihal.png', 0, 0, 0, 1, '2011-11-11', 13370, 0, '2017-01-01 00:00:00', 0), \
('100hitz---New-Country', '100hitz.com is an online radio network of free internet radio stations, featuring multiple genres of free internet radio stations, with the best in online radio and steaming audio music entertainment.', 'http://66.216.18.247:2186', 'http://www.100hitz.com/', '100hitz---New-Country.png', 0, 0, 0, 1, '2011-11-11', 13371, 0, '2017-01-01 00:00:00', 0), \
('Mas-FM-941', '', 'http://174.122.121.106:8000/mas94-ogg.ogg', 'http://www.cincoradio.com.mx/emisoras.asp?emi=XHJE', 'Mas-FM-941.png', 0, 0, 0, 1, '2011-11-11', 13372, 0, '2017-01-01 00:00:00', 0), \
('Book-Reading', '', 'http://174.122.121.106:8000/mas94-ogg.ogg', '', 'Book-Reading.png', 0, 0, 0, 1, '2011-11-11', 13373, 0, '2017-01-01 00:00:00', 0), \
('WEBY-1330', '', 'http://mega5.radioserver.co.uk:8026', 'http://www.1330weby.com/', 'WEBY-1330.png', 0, 0, 0, 1, '2011-11-11', 13374, 0, '2017-01-01 00:00:00', 0), \
('Streekradio-1048', '', 'http://livestream.streekradio.com/live', 'http://www.streekradio.com/', 'Streekradio-1048.png', 0, 0, 0, 1, '2011-11-11', 13375, 0, '2017-01-01 00:00:00', 0), \
('Z-Rock-103-1033', '', 'mms://live.cumulusstreaming.com/WXZZ-FM', 'http://www.zrock103.com/', 'Z-Rock-103-1033.png', 0, 0, 0, 1, '2011-11-11', 13376, 0, '2017-01-01 00:00:00', 0), \
('Healthline', '', 'mms://live.cumulusstreaming.com/WXZZ-FM', '', 'Healthline.png', 0, 0, 0, 1, '2011-11-11', 13378, 0, '2017-01-01 00:00:00', 0), \
('Radio-Direito-de-Viver', '', 'http://rdireitodeviver.localmidia.com.br:8050/aovivo', 'http://www.cliquecontraocancer.com.br/direitodeviver/', 'Radio-Direito-de-Viver.png', 0, 0, 0, 1, '2011-11-11', 13379, 0, '2017-01-01 00:00:00', 0), \
('80s-And-More', '80s Hits from the UK, USA, Aus &amp; Europe 24 hours a day', 'http://91.121.91.172:8518', 'http://www.80sandmore.org.uk/', '80s-And-More.png', 0, 0, 0, 1, '2011-11-11', 13380, 0, '2017-01-01 00:00:00', 0), \
('KIKS-FM-1015', 'ola radio broadcasting began on July 25, 1961 when 1370 AM KALN (KIOL&#39;s former call letters) signed on the air. 1370 AM was a daytime-only operation, as were many radio stations in small towns at ', 'mms://streamer2.securenetsystems.net/KIKS', 'http://www.iolaradio.com/', 'KIKS-FM-1015.png', 0, 0, 0, 1, '2011-11-11', 13381, 0, '2017-01-01 00:00:00', 0), \
('Jill-Taylor', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Jill-Taylor.png', 0, 0, 0, 1, '2011-11-11', 13382, 0, '2017-01-01 00:00:00', 0), \
('KNZZ-1100', '', 'mms://streamer2.securenetsystems.net/KIKS', 'http://www.1100knzz.com/', 'KNZZ-1100.png', 0, 0, 0, 1, '2011-11-11', 13383, 0, '2017-01-01 00:00:00', 0), \
('Eternal', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Eternal.png', 0, 0, 0, 1, '2011-11-11', 13384, 0, '2017-01-01 00:00:00', 0), \
('Prep-Sports-Report', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Prep-Sports-Report.png', 0, 0, 0, 1, '2011-11-11', 13385, 0, '2017-01-01 00:00:00', 0), \
('Into-the-Night-with-Tony-Bruno', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Into-the-Night-with-Tony-Bruno.png', 0, 0, 0, 1, '2011-11-11', 13386, 0, '2017-01-01 00:00:00', 0), \
('MOJO-1029', '', 'mms://streamer2.securenetsystems.net/KIKS', 'http://www.mojo1029.com', 'MOJO-1029.png', 0, 0, 0, 1, '2011-11-11', 13387, 0, '2017-01-01 00:00:00', 0), \
('Sky-Racing-Radios-Inside-Running', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Sky-Racing-Radios-Inside-Running.png', 0, 0, 0, 1, '2011-11-11', 13388, 0, '2017-01-01 00:00:00', 0), \
('973-The-Hawk', '', 'mms://streamer2.securenetsystems.net/KIKS', 'http://www.973thehawk.com/', '973-The-Hawk.png', 0, 0, 0, 1, '2011-11-11', 13389, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Evening-Jazz-with-Erik-Troe', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Sunday-Evening-Jazz-with-Erik-Troe.png', 0, 0, 0, 1, '2011-11-11', 13390, 0, '2017-01-01 00:00:00', 0), \
('Marc-Anthony', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Marc-Anthony.png', 0, 0, 0, 1, '2011-11-11', 13392, 0, '2017-01-01 00:00:00', 0), \
('MUSIC-AND-SERVICE-ANNOUNCEMENTS', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'MUSIC-AND-SERVICE-ANNOUNCEMENTS.png', 0, 0, 0, 1, '2011-11-11', 13393, 0, '2017-01-01 00:00:00', 0), \
('Bridges-with-Africa', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Bridges-with-Africa.png', 0, 0, 0, 1, '2011-11-11', 13394, 0, '2017-01-01 00:00:00', 0), \
('Mix-929', '', 'mms://streamer2.securenetsystems.net/KIKS', 'http://www.mix929.com/', 'Mix-929.png', 0, 0, 0, 1, '2011-11-11', 13395, 0, '2017-01-01 00:00:00', 0), \
('Justin-Timberlake', '', 'mms://streamer2.securenetsystems.net/KIKS', '', 'Justin-Timberlake.png', 0, 0, 0, 1, '2011-11-11', 13396, 0, '2017-01-01 00:00:00', 0), \
('WAAV-980', 'The Heritage Talk Radio station of Wilmington, North Carolina, with the leaders in todays Conservative Talk!', 'mms://live.cumulusstreaming.com/WAAV-AM', 'http://www.980waav.com/', 'WAAV-980.png', 0, 0, 0, 1, '2011-11-11', 13397, 0, '2017-01-01 00:00:00', 0), \
('Health-Show', '', 'mms://live.cumulusstreaming.com/WAAV-AM', '', 'Health-Show.png', 0, 0, 0, 1, '2011-11-11', 13398, 0, '2017-01-01 00:00:00', 0), \
('Kiss-Canaries-994', '', 'http://s3.viastreaming.net:8570', 'http://www.kisscanaries.com', 'Kiss-Canaries-994.png', 0, 0, 0, 1, '2011-11-11', 13399, 0, '2017-01-01 00:00:00', 0), \
('The-Big-Sports-Breakfast', '', 'http://s3.viastreaming.net:8570', '', 'The-Big-Sports-Breakfast.png', 0, 0, 0, 1, '2011-11-11', 13400, 0, '2017-01-01 00:00:00', 0), \
('Moody-Radio-Network', 'Christian teaching and talk network feeding programming to hundreds of affiliates nationwide from studios at Moody Bible Institute in Chicago.', 'http://209.62.16.60:80/mbn1.mp3', 'http://www.moodyradio.org/', 'Moody-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 13401, 0, '2017-01-01 00:00:00', 0), \
('Derailment-Radio', '', 'http://209.62.16.60:80/mbn1.mp3', '', 'Derailment-Radio.png', 0, 0, 0, 1, '2011-11-11', 13402, 0, '2017-01-01 00:00:00', 0), \
('Beherit', '', 'http://209.62.16.60:80/mbn1.mp3', '', 'Beherit.png', 0, 0, 0, 1, '2011-11-11', 13403, 0, '2017-01-01 00:00:00', 0), \
('The-Average-Joe-Show', '', 'http://209.62.16.60:80/mbn1.mp3', '', 'The-Average-Joe-Show.png', 0, 0, 0, 1, '2011-11-11', 13404, 0, '2017-01-01 00:00:00', 0), \
('The-Pulse-1400', '', 'mms://nick9.surfernetwork.com/WTSL', 'http://www.wtsl.com/', 'The-Pulse-1400.png', 0, 0, 0, 1, '2011-11-11', 13405, 0, '2017-01-01 00:00:00', 0), \
('Gen-X-Radio-1067', '', 'mms://nick9.surfernetwork.com/WTSL', 'http://www.genxcolumbus.com/', 'Gen-X-Radio-1067.png', 0, 0, 0, 1, '2011-11-11', 13406, 0, '2017-01-01 00:00:00', 0), \
('Sinatra-Style-on-JAZZRADIOcom', 'For fans of Frank Sinatra, this channel is full of great American standards.', 'http://listen.jazzradio.com/partner_aac/sinatrastyle.pls', 'http://www.jazzradio.com/', 'Sinatra-Style-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 13407, 0, '2017-01-01 00:00:00', 0), \
('Witnesses-of-Faith', '', 'http://listen.jazzradio.com/partner_aac/sinatrastyle.pls', '', 'Witnesses-of-Faith.png', 0, 0, 0, 1, '2011-11-11', 13408, 0, '2017-01-01 00:00:00', 0), \
('War', '', 'http://listen.jazzradio.com/partner_aac/sinatrastyle.pls', '', 'War.png', 0, 0, 0, 1, '2011-11-11', 13409, 0, '2017-01-01 00:00:00', 0), \
('In-Memory-Of', '', 'http://listen.jazzradio.com/partner_aac/sinatrastyle.pls', '', 'In-Memory-Of.png', 0, 0, 0, 1, '2011-11-11', 13410, 0, '2017-01-01 00:00:00', 0), \
('MusikSystemet', '', 'http://listen.jazzradio.com/partner_aac/sinatrastyle.pls', '', 'MusikSystemet.png', 0, 0, 0, 1, '2011-11-11', 13411, 0, '2017-01-01 00:00:00', 0), \
('Virgin-Radio-Nouveauts', '', 'http://vipicecast.yacast.net/vra_webradio06.m3u', 'http://www.virginradio.fr/', 'Virgin-Radio-Nouveauts.png', 0, 0, 0, 1, '2011-11-11', 13412, 0, '2017-01-01 00:00:00', 0), \
('WRHIcom-Football-Stream-3', '', 'http://vipicecast.yacast.net/vra_webradio06.m3u', 'http://www.wrhi.com', 'WRHIcom-Football-Stream-3.png', 0, 0, 0, 1, '2011-11-11', 13413, 0, '2017-01-01 00:00:00', 0), \
('WPTL-920', 'WPTL (920 AM) is a radio station broadcasting a Country music format, including ABC Radio Networks Real Country, and licensed to Canton, North Carolina, USA. The station is currently owned by Skycount', 'mms://varmedia.automatedresults.net/wptl', 'http://wptlradio.com/', 'WPTL-920.png', 0, 0, 0, 1, '2011-11-11', 13414, 0, '2017-01-01 00:00:00', 0), \
('Fresh-1003', '', 'mms://varmedia.automatedresults.net/wptl', 'http://www.wnic.com/', 'Fresh-1003.png', 0, 0, 0, 1, '2011-11-11', 13415, 0, '2017-01-01 00:00:00', 0), \
('The-Source-1310', '', 'mms://varmedia.automatedresults.net/wptl', '', 'The-Source-1310.png', 0, 0, 0, 1, '2011-11-11', 13416, 0, '2017-01-01 00:00:00', 0), \
('KIA-FM-939', '', 'http://icy2.abacast.com/threeeagles-mcwkiaifm-32.m3u', 'http://www.kiaifm.com/', 'KIA-FM-939.png', 0, 0, 0, 1, '2011-11-11', 13417, 0, '2017-01-01 00:00:00', 0), \
('Radio-Metro-1024', '', 'http://195.182.132.18:8230', 'http://radiometro.ru/', 'Radio-Metro-1024.png', 0, 0, 0, 1, '2011-11-11', 13418, 0, '2017-01-01 00:00:00', 0), \
('The-Juan-MacLean', '', 'http://195.182.132.18:8230', '', 'The-Juan-MacLean.png', 0, 0, 0, 1, '2011-11-11', 13419, 0, '2017-01-01 00:00:00', 0), \
('Weekly-Top-40', '', 'http://195.182.132.18:8230', '', 'Weekly-Top-40.png', 0, 0, 0, 1, '2011-11-11', 13420, 0, '2017-01-01 00:00:00', 0), \
('Lazer-993', '', 'http://195.182.132.18:8230', 'http://www.lazer993.com/', 'Lazer-993.png', 0, 0, 0, 1, '2011-11-11', 13421, 0, '2017-01-01 00:00:00', 0), \
('Thunder-102-1021', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', 'http://www.thunder102.com/', 'Thunder-102-1021.png', 0, 0, 0, 1, '2011-11-11', 13422, 0, '2017-01-01 00:00:00', 0), \
('American-Top-40', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', '', 'American-Top-40.png', 0, 0, 0, 1, '2011-11-11', 13423, 0, '2017-01-01 00:00:00', 0), \
('PRIMEtime', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', '', 'PRIMEtime.png', 0, 0, 0, 1, '2011-11-11', 13424, 0, '2017-01-01 00:00:00', 0), \
('K-953', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', 'http://www.k-musicradio.com/', 'K-953.png', 0, 0, 0, 1, '2011-11-11', 13425, 0, '2017-01-01 00:00:00', 0), \
('WXLO-1045', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', 'http://www.wxlo.com/', 'WXLO-1045.png', 0, 0, 0, 1, '2011-11-11', 13426, 0, '2017-01-01 00:00:00', 0), \
('A-different-idea', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', '', 'A-different-idea.png', 0, 0, 0, 1, '2011-11-11', 13427, 0, '2017-01-01 00:00:00', 0), \
('Yellow-Jackets-Update', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', '', 'Yellow-Jackets-Update.png', 0, 0, 0, 1, '2011-11-11', 13428, 0, '2017-01-01 00:00:00', 0), \
('WSOO-1230', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', 'http://www.1230wsoo.com/', 'WSOO-1230.png', 0, 0, 0, 1, '2011-11-11', 13429, 0, '2017-01-01 00:00:00', 0), \
('Sportstalk-790', '', 'http://crystalout.surfernetwork.com:8001/WDNB_MP3', 'http://www.sports790.com', 'Sportstalk-790.png', 0, 0, 0, 1, '2011-11-11', 13430, 0, '2017-01-01 00:00:00', 0), \
('MSA-Sports--PIHL-Hockey', '', 'http://msasportsnetwork.com/asx.asp?id=28810type=a', '', 'MSA-Sports--PIHL-Hockey.png', 0, 0, 0, 1, '2011-11-11', 13431, 0, '2017-01-01 00:00:00', 0), \
('WATT-1240', '', 'http://msasportsnetwork.com/asx.asp?id=28810type=a', 'http://www.radiomgb.com/watt/', 'WATT-1240.png', 0, 0, 0, 1, '2011-11-11', 13433, 0, '2017-01-01 00:00:00', 0), \
('Shabba-Ranks', '', 'http://msasportsnetwork.com/asx.asp?id=28810type=a', '', 'Shabba-Ranks.png', 0, 0, 0, 1, '2011-11-11', 13434, 0, '2017-01-01 00:00:00', 0), \
('Hunt-Life-Outdoors-Show', '', 'http://msasportsnetwork.com/asx.asp?id=28810type=a', '', 'Hunt-Life-Outdoors-Show.png', 0, 0, 0, 1, '2011-11-11', 13435, 0, '2017-01-01 00:00:00', 0), \
('WAKE-1500', '', 'mms://stream3.securenetsystems.net/WAKE', 'http://www.wakeradio.com/', 'WAKE-1500.png', 0, 0, 0, 1, '2011-11-11', 13436, 0, '2017-01-01 00:00:00', 0), \
('Magic-104-1045', '', 'mms://stream3.securenetsystems.net/WAKE', 'http://www.conwaymagic.com/', 'Magic-104-1045.png', 0, 0, 0, 1, '2011-11-11', 13437, 0, '2017-01-01 00:00:00', 0), \
('Human-Nation-FM', '', 'http://178.77.67.179:8344/', 'http://www.hnfm.de/', 'Human-Nation-FM.png', 0, 0, 0, 1, '2011-11-11', 13438, 0, '2017-01-01 00:00:00', 0), \
('Light-of-Life-Job', '', 'http://178.77.67.179:8344/', '', 'Light-of-Life-Job.png', 0, 0, 0, 1, '2011-11-11', 13439, 0, '2017-01-01 00:00:00', 0), \
('Sua-trilha-4x4', '', 'http://178.77.67.179:8344/', '', 'Sua-trilha-4x4.png', 0, 0, 0, 1, '2011-11-11', 13440, 0, '2017-01-01 00:00:00', 0), \
('Kristians-Desired-Time', '', 'http://178.77.67.179:8344/', '', 'Kristians-Desired-Time.png', 0, 0, 0, 1, '2011-11-11', 13441, 0, '2017-01-01 00:00:00', 0), \
('MOViN-1049', '', 'http://sc1.abacast.com:9126', 'http://www.movin1049.com/', 'MOViN-1049.png', 0, 0, 0, 1, '2011-11-11', 13442, 0, '2017-01-01 00:00:00', 0), \
('Big-Band-And-Trad', '', 'http://sc1.abacast.com:9126', '', 'Big-Band-And-Trad.png', 0, 0, 1, 1, '2011-11-11', 13443, 0, '2011-10-14 16:04:20', 0), \
('WPEH-FM-921', '', 'http://sc1.abacast.com:9126', '', 'WPEH-FM-921.png', 0, 0, 0, 1, '2011-11-11', 13444, 0, '2017-01-01 00:00:00', 0), \
('Hardy-and-Trupiano', '', 'http://sc1.abacast.com:9126', '', 'Hardy-and-Trupiano.png', 0, 0, 0, 1, '2011-11-11', 13445, 0, '2017-01-01 00:00:00', 0), \
('KPRI-1021', 'KPRI-FM is owned by two guys.  That&#39;s it.  No big corporation, no Big Brother telling us what to do, and what to play..', 'http://sc1.abacast.com:9126', 'http://www.kprifm.com/', 'KPRI-1021.png', 0, 0, 0, 1, '2011-11-11', 13446, 0, '2017-01-01 00:00:00', 0), \
('Fun-Radio-Top-20', '24-hour stream 20 songs fun radio.', 'http://stream.funradio.sk:8000/top20128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-Top-20.png', 0, 0, 0, 1, '2011-11-11', 13447, 0, '2017-01-01 00:00:00', 0), \
('The-Sheep-540', '', 'http://wgth.serverroom.us:8652', 'http://www.wgth.net/', 'The-Sheep-540.png', 0, 0, 0, 1, '2011-11-11', 13448, 0, '2017-01-01 00:00:00', 0), \
('SODRE-CX6-R-Clasica-989', '', 'mms://200.40.120.90/01154', 'http://www.sodre.gub.uy/', 'SODRE-CX6-R-Clasica-989.png', 0, 0, 0, 1, '2011-11-11', 13449, 0, '2017-01-01 00:00:00', 0), \
('96-FM-961', '', 'mms://streaming.amnet.net.au/96fm', 'http://www.96fm.com.au/', '96-FM-961.png', 0, 0, 0, 1, '2011-11-11', 13450, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Temple-Times', '', 'mms://streaming.amnet.net.au/96fm', '', 'Sunday-Temple-Times.png', 0, 0, 0, 1, '2011-11-11', 13451, 0, '2017-01-01 00:00:00', 0), \
('Judas-Priest', '', 'mms://streaming.amnet.net.au/96fm', '', 'Judas-Priest.png', 0, 0, 0, 1, '2011-11-11', 13452, 0, '2017-01-01 00:00:00', 0), \
('WPTF-680', '', 'mms://streaming.amnet.net.au/96fm', 'http://www.wptf.com/', 'WPTF-680.png', 0, 0, 0, 1, '2011-11-11', 13453, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Indie-Rock', 'For discovering brand-new independent music and hearing stuff you may have missed, by emerging and established college and indie rock artists that don&#39;t fit into the mainstream.', 'http://streampoint.radioio.com/streams/760/46f28fc4c4be5/listen.pls', 'http://www.radioio.com/channels/indie-rock', 'RadioIO-Indie-Rock.png', 0, 0, 0, 1, '2011-11-11', 13454, 0, '2017-01-01 00:00:00', 0), \
('Jazz-at-Dawn', '', 'http://streampoint.radioio.com/streams/760/46f28fc4c4be5/listen.pls', '', 'Jazz-at-Dawn.png', 0, 0, 0, 1, '2011-11-11', 13455, 0, '2017-01-01 00:00:00', 0), \
('Dada-Life', '', 'http://streampoint.radioio.com/streams/760/46f28fc4c4be5/listen.pls', '', 'Dada-Life.png', 0, 0, 0, 1, '2011-11-11', 13456, 0, '2017-01-01 00:00:00', 0), \
('Joes-Jazz', '', 'http://streampoint.radioio.com/streams/760/46f28fc4c4be5/listen.pls', '', 'Joes-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13457, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1230', '', 'http://173.244.194.212:8013', 'http://espnfay.com/', 'ESPN-Radio-1230.png', 0, 0, 0, 1, '2011-11-11', 13458, 0, '2017-01-01 00:00:00', 0), \
('Coco-Montoya', '', 'http://173.244.194.212:8013', '', 'Coco-Montoya.png', 0, 0, 0, 1, '2011-11-11', 13459, 0, '2017-01-01 00:00:00', 0), \
('DI-Lounge', 'Sit back and enjoy the lounge grooves!', 'http://listen.di.fm/partner_mp3/lounge.pls', 'http://www.di.fm/lounge', 'DI-Lounge.png', 0, 0, 0, 1, '2011-11-11', 13460, 0, '2017-01-01 00:00:00', 0), \
('SportsLine-Radio-Network', '', 'mms://nick9.surfernetwork.com/VALRUGER', 'http://slrnradiosports.com/', 'SportsLine-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 13461, 0, '2017-01-01 00:00:00', 0), \
('WOKV-690', '', 'mms://nick9.surfernetwork.com/VALRUGER', 'http://www.wokv.com/', 'WOKV-690.png', 0, 0, 0, 1, '2011-11-11', 13462, 0, '2017-01-01 00:00:00', 0), \
('Lunch-with-Lewi-McKirdy', '', 'mms://nick9.surfernetwork.com/VALRUGER', '', 'Lunch-with-Lewi-McKirdy.png', 0, 0, 0, 1, '2011-11-11', 13463, 0, '2017-01-01 00:00:00', 0);";


const char *radio_station_setupsql42="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('LBC-973', '', 'http://mediaweb.musicradio.com/show.asx?StreamID=156', 'http://www.lbc.co.uk/', 'LBC-973.png', 0, 0, 0, 1, '2011-11-11', 13464, 0, '2017-01-01 00:00:00', 0), \
('Radio-Carlin', '', 'http://84.40.30.145:8008/', 'http://www.remembercarlin.com/', 'Radio-Carlin.png', 0, 0, 0, 1, '2011-11-11', 13465, 0, '2017-01-01 00:00:00', 0), \
('Racing-Ahead', '', 'http://84.40.30.145:8008/', '', 'Racing-Ahead.png', 0, 0, 0, 1, '2011-11-11', 13466, 0, '2017-01-01 00:00:00', 0), \
('La-Gran-D-1340', 'La GranD 1340 AM es la estaci&#243;n de los Hits de Regional Mexicana con el Piolin por la Ma&#241;ana.', 'http://84.40.30.145:8008/', 'http://www.lagrand1340kc.com', 'La-Gran-D-1340.png', 0, 0, 0, 1, '2011-11-11', 13467, 0, '2017-01-01 00:00:00', 0), \
('KCBF-820', '', 'http://ondemand.nnbradio.net/audio/nnbfairbanks-kcbf-48.m3u', 'http://www.820sports.com/', 'KCBF-820.png', 0, 0, 0, 1, '2011-11-11', 13469, 0, '2017-01-01 00:00:00', 0), \
('Indiana-Hoosiers-at-Iowa-Hawkeyes-Oct-22-2011', '', 'http://ondemand.nnbradio.net/audio/nnbfairbanks-kcbf-48.m3u', '', 'Indiana-Hoosiers-at-Iowa-Hawkeyes-Oct-22-2011.png', 0, 0, 0, 1, '2011-11-11', 13470, 0, '2017-01-01 00:00:00', 0), \
('Q1019', '', 'http://ondemand.nnbradio.net/audio/nnbfairbanks-kcbf-48.m3u', 'http://www.q1019.com/', 'Q1019.png', 0, 0, 0, 1, '2011-11-11', 13471, 0, '2017-01-01 00:00:00', 0), \
('WTSN-Sunday-Morning-Information-Center', '', 'http://ondemand.nnbradio.net/audio/nnbfairbanks-kcbf-48.m3u', '', 'WTSN-Sunday-Morning-Information-Center.png', 0, 0, 0, 1, '2011-11-11', 13472, 0, '2017-01-01 00:00:00', 0), \
('Lets-Talk-About-Jesus', '', 'http://ondemand.nnbradio.net/audio/nnbfairbanks-kcbf-48.m3u', '', 'Lets-Talk-About-Jesus.png', 0, 0, 0, 1, '2011-11-11', 13473, 0, '2017-01-01 00:00:00', 0), \
('Gods-Living-Word', '', 'http://ondemand.nnbradio.net/audio/nnbfairbanks-kcbf-48.m3u', '', 'Gods-Living-Word.png', 0, 0, 0, 1, '2011-11-11', 13474, 0, '2017-01-01 00:00:00', 0), \
('Arrow-FM-1078', 'Arrow FM plays a selection of hit songs from the last forty years, and offers regular updates on local news, sport, travel and events. The station also works with a number of local charities and organ', 'http://78.129.232.99/arrowfm?.wma', 'http://www.arrowfm.co.uk/', 'Arrow-FM-1078.png', 0, 0, 0, 1, '2011-11-11', 13475, 0, '2017-01-01 00:00:00', 0), \
('Del-Amitri', '', 'http://78.129.232.99/arrowfm?.wma', '', 'Del-Amitri.png', 0, 0, 0, 1, '2011-11-11', 13476, 0, '2017-01-01 00:00:00', 0), \
('Easy-Network-987', 'Easy Network , storica emittente di Veneto e Friuli Venezia Giulia, grazie alle sua diffusione capillare e alla sua programmazione musicale', 'http://str01.fluidstream.net:6030', 'http://www.easynetwork.fm/', 'Easy-Network-987.png', 0, 0, 0, 1, '2011-11-11', 13477, 0, '2017-01-01 00:00:00', 0), \
('KSUE-Religious-Programming', '', 'http://str01.fluidstream.net:6030', '', 'KSUE-Religious-Programming.png', 0, 0, 0, 1, '2011-11-11', 13478, 0, '2017-01-01 00:00:00', 0), \
('RFD-Illinois', '', 'http://str01.fluidstream.net:6030', '', 'RFD-Illinois.png', 0, 0, 0, 1, '2011-11-11', 13479, 0, '2017-01-01 00:00:00', 0), \
('Houndog-Radio', '', 'http://jacksontrailmedia.com:8012/', 'http://www.hounddogradio.net/', 'Houndog-Radio.png', 0, 0, 0, 1, '2011-11-11', 13480, 0, '2017-01-01 00:00:00', 0), \
('Armand-van-Helden', '', 'http://jacksontrailmedia.com:8012/', '', 'Armand-van-Helden.png', 0, 0, 0, 1, '2011-11-11', 13481, 0, '2017-01-01 00:00:00', 0), \
('KCOX-1350', '', 'http://jacksontrailmedia.com:8012/', 'http://www.1027ktxj.com/', 'KCOX-1350.png', 0, 0, 0, 1, '2011-11-11', 13482, 0, '2017-01-01 00:00:00', 0), \
('WJMA-FM-1031', '', 'http://jacksontrailmedia.com:8012/', 'http://www.wjmafm.com/', 'WJMA-FM-1031.png', 0, 0, 0, 1, '2011-11-11', 13483, 0, '2017-01-01 00:00:00', 0), \
('WABH-1380', '', 'http://jacksontrailmedia.com:8012/', 'http://www.1380wabh.com/', 'WABH-1380.png', 0, 0, 0, 1, '2011-11-11', 13484, 0, '2017-01-01 00:00:00', 0), \
('Virgin-Radio-Pop-Rock', '', 'http://vipicecast.yacast.net/vra_webradio01.m3u', 'http://www.virginradio.fr', 'Virgin-Radio-Pop-Rock.png', 0, 0, 0, 1, '2011-11-11', 13485, 0, '2017-01-01 00:00:00', 0), \
('Burning-Spear', '', 'http://vipicecast.yacast.net/vra_webradio01.m3u', '', 'Burning-Spear.png', 0, 0, 0, 1, '2011-11-11', 13486, 0, '2017-01-01 00:00:00', 0), \
('Funeral-Party', '', 'http://vipicecast.yacast.net/vra_webradio01.m3u', '', 'Funeral-Party.png', 0, 0, 0, 1, '2011-11-11', 13487, 0, '2017-01-01 00:00:00', 0), \
('NonStopNeoncom', 'A radio station from the UK playing Brighter Music from the 80s &amp; 90s.', 'http://stream.nonstopneon.com/tunein.php/nonstopneon/playlist.pls', 'http://www.nonstopneon.com/', 'NonStopNeoncom.png', 0, 0, 0, 1, '2011-11-11', 13488, 0, '2017-01-01 00:00:00', 0), \
('Modern-Music-and-More', '', 'http://stream.nonstopneon.com/tunein.php/nonstopneon/playlist.pls', '', 'Modern-Music-and-More.png', 0, 0, 0, 1, '2011-11-11', 13489, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Fantasy-Focus-Basketball-Podcast', '', 'http://stream.nonstopneon.com/tunein.php/nonstopneon/playlist.pls', '', 'ESPN-Fantasy-Focus-Basketball-Podcast.png', 0, 0, 0, 1, '2011-11-11', 13490, 0, '2017-01-01 00:00:00', 0), \
('KWSN-Infomercial', '', 'http://stream.nonstopneon.com/tunein.php/nonstopneon/playlist.pls', '', 'KWSN-Infomercial.png', 0, 0, 0, 1, '2011-11-11', 13491, 0, '2017-01-01 00:00:00', 0), \
('Radio-64', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.radio64fm.fr/', 'Radio-64.png', 0, 0, 0, 1, '2011-11-11', 13492, 0, '2017-01-01 00:00:00', 0), \
('Clay-JD-Walker', '', 'http://srv2.streaming-ingenierie.fr:8142/', '', 'Clay-JD-Walker.png', 0, 0, 0, 1, '2011-11-11', 13493, 0, '2017-01-01 00:00:00', 0), \
('The-Touch-923', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.carolinatouch.com/', 'The-Touch-923.png', 0, 0, 0, 1, '2011-11-11', 13494, 0, '2017-01-01 00:00:00', 0), \
('George-Strait', '', 'http://srv2.streaming-ingenierie.fr:8142/', '', 'George-Strait.png', 0, 0, 0, 1, '2011-11-11', 13495, 0, '2017-01-01 00:00:00', 0), \
('Groovin-1580', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.groovin1580.com/', 'Groovin-1580.png', 0, 0, 0, 1, '2011-11-11', 13496, 0, '2017-01-01 00:00:00', 0), \
('Motor-Directo', '', 'http://srv2.streaming-ingenierie.fr:8142/', '', 'Motor-Directo.png', 0, 0, 0, 1, '2011-11-11', 13497, 0, '2017-01-01 00:00:00', 0), \
('WTJK-1380', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.espn1380.com/', 'WTJK-1380.png', 0, 0, 0, 1, '2011-11-11', 13498, 0, '2017-01-01 00:00:00', 0), \
('WGAR-FM-995', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.wgar.com/', 'WGAR-FM-995.png', 0, 0, 0, 1, '2011-11-11', 13499, 0, '2017-01-01 00:00:00', 0), \
('Star-951', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.wcdzradio.com/', 'Star-951.png', 0, 0, 0, 1, '2011-11-11', 13500, 0, '2017-01-01 00:00:00', 0), \
('K-Bay-945', '', 'http://srv2.streaming-ingenierie.fr:8142/', 'http://www.kbay.com/', 'K-Bay-945.png', 0, 0, 0, 1, '2011-11-11', 13501, 0, '2017-01-01 00:00:00', 0), \
('KYCA-1490', '', 'http://streaming.kyca.info:7120/listen.pls', 'http://www.kyca.info/kyca.php', 'KYCA-1490.png', 0, 0, 0, 1, '2011-11-11', 13502, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-995', '', 'mms://live.cumulusstreaming.com/KPLX-FM', 'http://www.995thewolf.com/', 'The-Wolf-995.png', 0, 0, 0, 1, '2011-11-11', 13503, 0, '2017-01-01 00:00:00', 0), \
('The-Sisters-of-Mercy', '', 'mms://live.cumulusstreaming.com/KPLX-FM', '', 'The-Sisters-of-Mercy.png', 0, 0, 0, 1, '2011-11-11', 13504, 0, '2017-01-01 00:00:00', 0), \
('All-Over-the-Shop', '', 'mms://live.cumulusstreaming.com/KPLX-FM', '', 'All-Over-the-Shop.png', 0, 0, 0, 1, '2011-11-11', 13505, 0, '2017-01-01 00:00:00', 0), \
('Joe-Lovano', '', 'mms://live.cumulusstreaming.com/KPLX-FM', '', 'Joe-Lovano.png', 0, 0, 0, 1, '2011-11-11', 13506, 0, '2017-01-01 00:00:00', 0), \
('Golds-Greatest-Hits', '', 'mms://live.cumulusstreaming.com/KPLX-FM', '', 'Golds-Greatest-Hits.png', 0, 0, 0, 1, '2011-11-11', 13507, 0, '2017-01-01 00:00:00', 0), \
('NASCAR-USA', '', 'mms://live.cumulusstreaming.com/KPLX-FM', '', 'NASCAR-USA.png', 0, 0, 0, 1, '2011-11-11', 13508, 0, '2017-01-01 00:00:00', 0), \
('CKUA-FM-949', '', 'mms://live.cumulusstreaming.com/KPLX-FM', 'http://www.ckua.com/', 'CKUA-FM-949.png', 0, 0, 0, 1, '2011-11-11', 13509, 0, '2017-01-01 00:00:00', 0), \
('Halloween-Radio', 'Scary spooky horror thrilling Halloween radio.', 'http://www.halloweenradio.net/streaming/halloweenradio.m3u', 'http://www.halloweenradio.net/', 'Halloween-Radio.png', 0, 0, 0, 1, '2011-11-11', 13510, 0, '2017-01-01 00:00:00', 0), \
('Kids-Public-Radio-Pipsqueaks', '', 'http://www.kidspublicradio.org:8000/pipsqueaks', 'http://www.kidspublicradio.org/', 'Kids-Public-Radio-Pipsqueaks.png', 0, 0, 0, 1, '2011-11-11', 13511, 0, '2017-01-01 00:00:00', 0), \
('Peter-Bjorn-and-John', '', 'http://www.kidspublicradio.org:8000/pipsqueaks', '', 'Peter-Bjorn-and-John.png', 0, 0, 0, 1, '2011-11-11', 13512, 0, '2017-01-01 00:00:00', 0), \
('WTIF-FM-1075', '', 'http://mercury.friendlycity.net:8004', 'http://www.wtif1075.com', 'WTIF-FM-1075.png', 0, 0, 0, 1, '2011-11-11', 13513, 0, '2017-01-01 00:00:00', 0), \
('Atmosphere', '', 'http://mercury.friendlycity.net:8004', '', 'Atmosphere.png', 0, 0, 0, 1, '2011-11-11', 13514, 0, '2017-01-01 00:00:00', 0), \
('Deluxe-Radio', 'DELUXE RADIO ist das erste Radioprogramm das Ihnen 24 Stunden Musik pur bringt.', 'http://deluxetelevision.com/livestreams/radio/DELUXE_RADIO.pls', 'http://www.radiodeluxe.de/', 'Deluxe-Radio.png', 0, 0, 0, 1, '2011-11-11', 13515, 0, '2017-01-01 00:00:00', 0), \
('AccuRadio-Adult-Alternative-Double-Latte', '', 'http://www.accuradio.com/shoutcast/links/double_latte.pls', 'http://www.accuradio.com/classical/', '', 0, 0, 0, 1, '2011-11-11', 13516, 0, '2017-01-01 00:00:00', 0), \
('WEOL-Morning-Show', '', 'http://www.accuradio.com/shoutcast/links/double_latte.pls', '', 'WEOL-Morning-Show.png', 0, 0, 0, 1, '2011-11-11', 13517, 0, '2017-01-01 00:00:00', 0), \
('Classy-959', '', 'http://streams.midutahradio.com:8000/kmgr', 'http://midutahradio.com/kmgr', 'Classy-959.png', 0, 0, 0, 1, '2011-11-11', 13518, 0, '2017-01-01 00:00:00', 0), \
('Modern-Music-and-More', '', 'http://streams.midutahradio.com:8000/kmgr', '', 'Modern-Music-and-More.png', 0, 0, 0, 1, '2011-11-11', 13519, 0, '2017-01-01 00:00:00', 0), \
('Tom-Harrell', '', 'http://streams.midutahradio.com:8000/kmgr', '', 'Tom-Harrell.png', 0, 0, 0, 1, '2011-11-11', 13520, 0, '2017-01-01 00:00:00', 0), \
('Lostprophets', '', 'http://streams.midutahradio.com:8000/kmgr', '', 'Lostprophets.png', 0, 0, 0, 1, '2011-11-11', 13521, 0, '2017-01-01 00:00:00', 0), \
('Tina-Cousins', '', 'http://streams.midutahradio.com:8000/kmgr', '', 'Tina-Cousins.png', 0, 0, 0, 1, '2011-11-11', 13522, 0, '2017-01-01 00:00:00', 0), \
('Star-1007', 'STAR 100.7 Today&#39;s Variety', 'http://streams.midutahradio.com:8000/kmgr', 'http://www.1007.com/', 'Star-1007.png', 0, 0, 0, 1, '2011-11-11', 13523, 0, '2017-01-01 00:00:00', 0), \
('Randy-Owen', '', 'http://streams.midutahradio.com:8000/kmgr', '', 'Randy-Owen.png', 0, 0, 0, 1, '2011-11-11', 13524, 0, '2017-01-01 00:00:00', 0), \
('domradiode-9675', 'domradio.de - der Sender des Erzbistums K&#246;ln - sendet seit Pfingsten 2000 und ist der erste kirchliche Sender in offizieller Tr&#228;gerschaft eines Bistums. Dabei ist der Sendername auch ein St&', 'http://stream.domradio.de/audio/radio.mp3', 'http://www.domradio.de/', 'domradiode-9675.png', 0, 0, 0, 1, '2011-11-11', 13525, 0, '2017-01-01 00:00:00', 0), \
('Baseball-Today', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'Baseball-Today.png', 0, 0, 0, 1, '2011-11-11', 13526, 0, '2017-01-01 00:00:00', 0), \
('Columbus-Blue-Jackets-at-Dallas-Stars-Oct-15-2011', '', 'http://stream.domradio.de/audio/radio.mp3', '', '', 0, 0, 0, 1, '2011-11-11', 13528, 0, '2017-01-01 00:00:00', 0), \
('Majic-1057', 'Cleveland&#39;s Greatest Hits...&quot;Classic Hits&quot; station!', 'http://stream.domradio.de/audio/radio.mp3', 'http://www.wmji.com/', 'Majic-1057.png', 0, 0, 0, 1, '2011-11-11', 13529, 0, '2017-01-01 00:00:00', 0), \
('Gym-Class-Heroes', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'Gym-Class-Heroes.png', 0, 0, 0, 1, '2011-11-11', 13530, 0, '2017-01-01 00:00:00', 0), \
('Matthew-Sweet', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'Matthew-Sweet.png', 0, 0, 0, 1, '2011-11-11', 13531, 0, '2017-01-01 00:00:00', 0), \
('Focus-Kultur', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'Focus-Kultur.png', 0, 0, 0, 1, '2011-11-11', 13532, 0, '2017-01-01 00:00:00', 0), \
('LEN', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'LEN.png', 0, 0, 0, 1, '2011-11-11', 13533, 0, '2017-01-01 00:00:00', 0), \
('The-Heavy', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'The-Heavy.png', 0, 0, 0, 1, '2011-11-11', 13534, 0, '2017-01-01 00:00:00', 0), \
('Jon-Linder-Show', '', 'http://stream.domradio.de/audio/radio.mp3', '', 'Jon-Linder-Show.png', 0, 0, 0, 1, '2011-11-11', 13535, 0, '2017-01-01 00:00:00', 0), \
('BBC-Kent-967', '', 'http://www.bbc.co.uk/radio/listen/live/bbckent.asx', 'http://www.bbc.co.uk/kent/', 'BBC-Kent-967.png', 0, 0, 0, 1, '2011-11-11', 13536, 0, '2017-01-01 00:00:00', 0), \
('Sunshine-FM-994', '', 'http://195.56.193.90:8100', 'http://www.sunshinefm.hu/', 'Sunshine-FM-994.png', 0, 0, 0, 1, '2011-11-11', 13537, 0, '2017-01-01 00:00:00', 0), \
('Beenie-Man', '', 'http://195.56.193.90:8100', '', 'Beenie-Man.png', 0, 0, 0, 1, '2011-11-11', 13538, 0, '2017-01-01 00:00:00', 0), \
('Redeemer-Lutheran-Church', '', 'http://195.56.193.90:8100', '', 'Redeemer-Lutheran-Church.png', 0, 0, 0, 1, '2011-11-11', 13539, 0, '2017-01-01 00:00:00', 0), \
('The-Beautiful-South', '', 'http://195.56.193.90:8100', '', 'The-Beautiful-South.png', 0, 0, 0, 1, '2011-11-11', 13540, 0, '2017-01-01 00:00:00', 0), \
('Martial-Arts-Legends', '', 'http://195.56.193.90:8100', '', 'Martial-Arts-Legends.png', 0, 0, 0, 1, '2011-11-11', 13541, 0, '2017-01-01 00:00:00', 0), \
('-and-the-Mysterians', '', 'http://195.56.193.90:8100', '', '', 0, 0, 0, 1, '2011-11-11', 13542, 0, '2017-01-01 00:00:00', 0), \
('Mix-1007', '', 'mms://live.cumulusstreaming.com/WNMX-FM', 'http://mix100fm.com/', 'Mix-1007.png', 0, 0, 0, 1, '2011-11-11', 13543, 0, '2017-01-01 00:00:00', 0), \
('Radio-Activa', 'Estaci&#243;n de la Universidad del Sagrado Coraz&#243;n', 'http://streaming.sagrado.edu:8000/listen', 'http://www.sagrado.edu/radioactiva/index.htm', 'Radio-Activa.png', 0, 0, 0, 1, '2011-11-11', 13544, 0, '2017-01-01 00:00:00', 0), \
('Chicago-Bears-at-Detroit-Lions-Oct-10-2011', '', 'http://streaming.sagrado.edu:8000/listen', '', '', 0, 0, 0, 1, '2011-11-11', 13546, 0, '2017-01-01 00:00:00', 0), \
('World-Cup-Drivetime', '', 'http://streaming.sagrado.edu:8000/listen', '', 'World-Cup-Drivetime.png', 0, 0, 0, 1, '2011-11-11', 13547, 0, '2017-01-01 00:00:00', 0), \
('The-John-Kincade-Show', '', 'http://streaming.sagrado.edu:8000/listen', '', 'The-John-Kincade-Show.png', 0, 0, 0, 1, '2011-11-11', 13548, 0, '2017-01-01 00:00:00', 0), \
('Bob-FM-1039', '', 'http://streaming.sagrado.edu:8000/listen', 'http://www.1039bobfm.com/', 'Bob-FM-1039.png', 0, 0, 0, 1, '2011-11-11', 13549, 0, '2017-01-01 00:00:00', 0), \
('Five', '', 'http://streaming.sagrado.edu:8000/listen', '', 'Five.png', 0, 0, 0, 1, '2011-11-11', 13550, 0, '2017-01-01 00:00:00', 0), \
('WARF-1350', '', 'http://streaming.sagrado.edu:8000/listen', 'http://www.sportsradio1350.com/main.html', 'WARF-1350.png', 0, 0, 0, 1, '2011-11-11', 13551, 0, '2017-01-01 00:00:00', 0), \
('Deerhunter', '', 'http://streaming.sagrado.edu:8000/listen', '', 'Deerhunter.png', 0, 0, 0, 1, '2011-11-11', 13552, 0, '2017-01-01 00:00:00', 0), \
('Towards-London-2012-Olympic-and-Paralympic-programme', '', 'http://streaming.sagrado.edu:8000/listen', '', 'Towards-London-2012-Olympic-and-Paralympic-programme.png', 0, 0, 0, 1, '2011-11-11', 13553, 0, '2017-01-01 00:00:00', 0), \
('KCTC-1320', '', 'http://streaming.sagrado.edu:8000/listen', 'http://www.espn1320.net', 'KCTC-1320.png', 0, 0, 0, 1, '2011-11-11', 13554, 0, '2017-01-01 00:00:00', 0), \
('WDMG-860', '', 'http://streaming.sagrado.edu:8000/listen', '', 'WDMG-860.png', 0, 0, 0, 1, '2011-11-11', 13555, 0, '2017-01-01 00:00:00', 0), \
('Ace-Frehley', '', 'http://streaming.sagrado.edu:8000/listen', '', 'Ace-Frehley.png', 0, 0, 0, 1, '2011-11-11', 13556, 0, '2017-01-01 00:00:00', 0), \
('xitos-FM-999', '', 'http://lortel28.lorini.net:1050/EXITOS999FM', 'http://www.exitosfm.com', 'xitos-FM-999.png', 0, 0, 0, 1, '2011-11-11', 13557, 0, '2017-01-01 00:00:00', 0), \
('Outdoors-This-Week', '', 'http://lortel28.lorini.net:1050/EXITOS999FM', '', 'Outdoors-This-Week.png', 0, 0, 0, 1, '2011-11-11', 13558, 0, '2017-01-01 00:00:00', 0), \
('KLOG-1490', '', 'http://icy1.abacast.com/klog-klogmp3-64.m3u', 'http://www.klog.com/', 'KLOG-1490.png', 0, 0, 0, 1, '2011-11-11', 13560, 0, '2017-01-01 00:00:00', 0), \
('Terror', '', 'http://icy1.abacast.com/klog-klogmp3-64.m3u', '', 'Terror.png', 0, 0, 0, 1, '2011-11-11', 13561, 0, '2017-01-01 00:00:00', 0), \
('Absolute-Hitz', '', 'http://absolutehitz.com/listen.pls', 'http://AbsoluteHitz.com', 'Absolute-Hitz.png', 0, 0, 0, 1, '2011-11-11', 13562, 0, '2017-01-01 00:00:00', 0), \
('PartyMix', '', 'http://absolutehitz.com/listen.pls', '', 'PartyMix.png', 0, 0, 0, 1, '2011-11-11', 13563, 0, '2017-01-01 00:00:00', 0), \
('RFI-Romania-935', '', 'http://85.204.231.219:7100', 'http://www.rfi.ro/', 'RFI-Romania-935.png', 0, 0, 0, 1, '2011-11-11', 13564, 0, '2017-01-01 00:00:00', 0), \
('The-Knowledge-Show-Live', 'Live radio broadcast from Baltimore, MD touching on topics of conspiracy, education, religious topics, family discussions, mature topics, psychology, politics, and more. The show is in a mixed format ', 'http://98.233.228.105:88/broadwave.m3u?src=1&rate=1', 'http://www.knowledgeshowlive.com', 'The-Knowledge-Show-Live.png', 0, 0, 0, 1, '2011-11-11', 13565, 0, '2017-01-01 00:00:00', 0), \
('PulsRadio-Dance-And-Trance', '', 'http://94.23.14.41/', 'http://www.pulsradio.com/', 'PulsRadio-Dance-And-Trance.png', 0, 0, 0, 1, '2011-11-11', 13566, 0, '2017-01-01 00:00:00', 0), \
('Jamiroquai', '', 'http://94.23.14.41/', '', 'Jamiroquai.png', 0, 0, 0, 1, '2011-11-11', 13567, 0, '2017-01-01 00:00:00', 0), \
('KYYK-983', '', 'http://icy2.abacast.com/youreasttexas-kyykfm-32.m3u', 'http://www.youreasttexas.com/', 'KYYK-983.png', 0, 0, 0, 1, '2011-11-11', 13568, 0, '2017-01-01 00:00:00', 0), \
('Muddy-Waters', '', 'http://icy2.abacast.com/youreasttexas-kyykfm-32.m3u', '', 'Muddy-Waters.png', 0, 0, 0, 1, '2011-11-11', 13569, 0, '2017-01-01 00:00:00', 0), \
('Fairchild-Radio-1470', '', 'mms://216.18.70.242/AM1470N', 'http://www.am1470.com/', 'Fairchild-Radio-1470.png', 0, 0, 0, 1, '2011-11-11', 13570, 0, '2017-01-01 00:00:00', 0), \
('Light-of-Life-Gods-plan-for-the-future', '', 'mms://216.18.70.242/AM1470N', '', 'Light-of-Life-Gods-plan-for-the-future.png', 0, 0, 0, 1, '2011-11-11', 13571, 0, '2017-01-01 00:00:00', 0), \
('Boerne-Church-Of-Christ', '', 'mms://216.18.70.242/AM1470N', '', 'Boerne-Church-Of-Christ.png', 0, 0, 0, 1, '2011-11-11', 13572, 0, '2017-01-01 00:00:00', 0), \
('KRLT-939', '', 'mms://216.18.70.242/AM1470N', 'http://www.krltfm.com/', 'KRLT-939.png', 0, 0, 0, 1, '2011-11-11', 13573, 0, '2017-01-01 00:00:00', 0), \
('Parachute-Club', '', 'mms://216.18.70.242/AM1470N', '', 'Parachute-Club.png', 0, 0, 0, 1, '2011-11-11', 13575, 0, '2017-01-01 00:00:00', 0), \
('1057-The-Fan', '105.7 The Fan is a Sports Radio station based in Baltimore, MD.  105.7 The Fan can be heard at radio.com', 'mms://216.18.70.242/AM1470N', 'http://www.1057thefan.com/', '1057-The-Fan.png', 0, 0, 0, 1, '2011-11-11', 13576, 0, '2017-01-01 00:00:00', 0), \
('Lloyd', '', 'mms://216.18.70.242/AM1470N', '', 'Lloyd.png', 0, 0, 0, 1, '2011-11-11', 13577, 0, '2017-01-01 00:00:00', 0), \
('News-Radio-1260', '', 'mms://216.18.70.242/AM1470N', 'http://www.wxce1260.com/', 'News-Radio-1260.png', 0, 0, 0, 1, '2011-11-11', 13578, 0, '2017-01-01 00:00:00', 0), \
('Fools-Gold', '', 'mms://216.18.70.242/AM1470N', '', 'Fools-Gold.png', 0, 0, 0, 1, '2011-11-11', 13579, 0, '2017-01-01 00:00:00', 0), \
('Ian-Blackley', '', 'http://fm999.info:8080/radio.ogg', '', 'Ian-Blackley.png', 0, 0, 0, 1, '2011-11-11', 13581, 0, '2017-01-01 00:00:00', 0), \
('Lonestar', '', 'http://fm999.info:8080/radio.ogg', '', 'Lonestar.png', 0, 0, 0, 1, '2011-11-11', 13582, 0, '2017-01-01 00:00:00', 0), \
('Back-on-Basin-Street', '', 'http://fm999.info:8080/radio.ogg', '', 'Back-on-Basin-Street.png', 0, 0, 1, 1, '2011-11-11', 13583, 0, '2011-10-14 16:02:04', 0), \
('Joseph-Level', '', 'http://fm999.info:8080/radio.ogg', '', 'Joseph-Level.png', 0, 0, 0, 1, '2011-11-11', 13584, 0, '2017-01-01 00:00:00', 0), \
('Golden-Radio', 'Golden Radio', 'http://87.117.203.226:8008', 'http://www.goldenradio.it/', 'Golden-Radio.png', 0, 0, 0, 1, '2011-11-11', 13585, 0, '2017-01-01 00:00:00', 0), \
('AFR-Talk-917', 'American Family Radio (AFR) is a network of more than 180 radio stations broadcasting Christian-oriented programming to over 40 states. AFR was launched by Rev. Donald Wildmon in 1991 as a ministry of', 'http://mediaserver3.afa.net:8000/talkhigh.mp3', 'http://www.afr.net/', 'AFR-Talk-917.png', 0, 0, 0, 1, '2011-11-11', 13586, 0, '2017-01-01 00:00:00', 0), \
('The-Rezny-Wealth-Report', '', 'http://mediaserver3.afa.net:8000/talkhigh.mp3', '', 'The-Rezny-Wealth-Report.png', 0, 0, 0, 1, '2011-11-11', 13587, 0, '2017-01-01 00:00:00', 0), \
('Faith-Evans', '', 'http://mediaserver3.afa.net:8000/talkhigh.mp3', '', 'Faith-Evans.png', 0, 0, 0, 1, '2011-11-11', 13588, 0, '2017-01-01 00:00:00', 0), \
('WKZO-590', '', 'http://mediaserver3.afa.net:8000/talkhigh.mp3', 'http://www.wkzo.com/', 'WKZO-590.png', 0, 0, 0, 1, '2011-11-11', 13589, 0, '2017-01-01 00:00:00', 0), \
('Radio-La-Chevere-1009', '', 'http://usa8-vn.mixstream.net:8138/', 'http://www.radiolachevere.info/', 'Radio-La-Chevere-1009.png', 0, 0, 0, 1, '2011-11-11', 13590, 0, '2017-01-01 00:00:00', 0), \
('Smooth-937', '', 'http://asx.abacast.com/redwoodempire-kjzymp3-32.pls', 'http://www.kjzy.com/', 'Smooth-937.png', 0, 0, 0, 1, '2011-11-11', 13591, 0, '2017-01-01 00:00:00', 0), \
('WKUL-921', 'WKUL (92.1 FM, &quot;Country K-92&quot;) is a radio station licensed to serve Cullman, Alabama. The station is owned by Jonathan Christian Corp.', 'http://www.wkul.com/streaming/listen.asx', 'http://www.wkul.com/', 'WKUL-921.png', 0, 0, 0, 1, '2011-11-11', 13592, 0, '2017-01-01 00:00:00', 0), \
('WSNJ-1240', '', 'http://75.151.189.78:12864', 'http://www.wsnjam.com/', 'WSNJ-1240.png', 0, 0, 0, 1, '2011-11-11', 13593, 0, '2017-01-01 00:00:00', 0), \
('Nirvana', '', 'http://75.151.189.78:12864', '', 'Nirvana.png', 0, 0, 0, 1, '2011-11-11', 13594, 0, '2017-01-01 00:00:00', 0), \
('The-Gospel-Spin', '', 'http://75.151.189.78:12864', '', 'The-Gospel-Spin.png', 0, 0, 0, 1, '2011-11-11', 13595, 0, '2017-01-01 00:00:00', 0), \
('Aa', '', 'http://75.151.189.78:12864', '', 'Aa.png', 0, 0, 0, 1, '2011-11-11', 13596, 0, '2017-01-01 00:00:00', 0), \
('1010-WINS', '1010 WINS is a News radio station based in New York, NY.  1010 WINS can be heard at radio.com', 'http://75.151.189.78:12864', 'http://www.wins.com/', '1010-WINS.png', 0, 0, 0, 1, '2011-11-11', 13597, 0, '2017-01-01 00:00:00', 0), \
('Dive-Bar-Jukebox-Radio', '', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.charlestonillustrated.com/radio.htm', 'Dive-Bar-Jukebox-Radio.png', 0, 0, 0, 1, '2011-11-11', 13598, 0, '2017-01-01 00:00:00', 0), \
('Jimmie-Rodgers', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Jimmie-Rodgers.png', 0, 0, 0, 1, '2011-11-11', 13599, 0, '2017-01-01 00:00:00', 0), \
('Disturbed', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Disturbed.png', 0, 0, 0, 1, '2011-11-11', 13600, 0, '2017-01-01 00:00:00', 0), \
('Tell-It-Again', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Tell-It-Again.png', 0, 0, 0, 1, '2011-11-11', 13601, 0, '2017-01-01 00:00:00', 0), \
('Jazz-Profiles', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Jazz-Profiles.png', 0, 0, 0, 1, '2011-11-11', 13602, 0, '2017-01-01 00:00:00', 0), \
('971-The-Ticket', 'Detroit&#39;s #1 Sports Station', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.971theticket.com', '971-The-Ticket.png', 0, 0, 0, 1, '2011-11-11', 13603, 0, '2017-01-01 00:00:00', 0), \
('Sounds-of-the-20th-Century', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Sounds-of-the-20th-Century.png', 0, 0, 0, 1, '2011-11-11', 13604, 0, '2017-01-01 00:00:00', 0), \
('WVHU-800', '', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.800wvhu.com/', 'WVHU-800.png', 0, 0, 0, 1, '2011-11-11', 13605, 0, '2017-01-01 00:00:00', 0), \
('KLOE-730', '', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.kloe.com/', 'KLOE-730.png', 0, 0, 0, 1, '2011-11-11', 13606, 0, '2017-01-01 00:00:00', 0), \
('Hot-Kiss-1063', '', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.hotkiss1063.com/', 'Hot-Kiss-1063.png', 0, 0, 0, 1, '2011-11-11', 13607, 0, '2017-01-01 00:00:00', 0), \
('WPKO-FM-983', 'WPKO-FM (98.3 FM) is an American radio station. It is programmed in a Hot AC radio format, and is almost entirely locally produced.', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.peakofohio.com/', 'WPKO-FM-983.png', 0, 0, 0, 1, '2011-11-11', 13608, 0, '2017-01-01 00:00:00', 0), \
('D-FM-902', '', 'http://striiming.trio.ee/dfm.mp3', 'http://www.dfm.ee/', 'D-FM-902.png', 0, 0, 0, 1, '2011-11-11', 13609, 0, '2017-01-01 00:00:00', 0), \
('Stevie-Storm', '', 'http://striiming.trio.ee/dfm.mp3', '', 'Stevie-Storm.png', 0, 0, 0, 1, '2011-11-11', 13610, 0, '2017-01-01 00:00:00', 0), \
('K971', '', 'http://striiming.trio.ee/dfm.mp3', 'http://www.k97fm.com/', 'K971.png', 0, 0, 0, 1, '2011-11-11', 13611, 0, '2017-01-01 00:00:00', 0), \
('Raw-FM-876', '', 'http://stream.rawfm.com.au:8009/listen.pls', 'http://www.rawfm.com.au/', 'Raw-FM-876.png', 0, 0, 0, 1, '2011-11-11', 13612, 0, '2017-01-01 00:00:00', 0), \
('The-Stone-Roses', '', 'http://stream.rawfm.com.au:8009/listen.pls', '', 'The-Stone-Roses.png', 0, 0, 0, 1, '2011-11-11', 13613, 0, '2017-01-01 00:00:00', 0), \
('Bunker-to-Bunker', '', 'http://stream.rawfm.com.au:8009/listen.pls', '', 'Bunker-to-Bunker.png', 0, 0, 0, 1, '2011-11-11', 13614, 0, '2017-01-01 00:00:00', 0), \
('Tambayan-1019', 'DWRR-FM, branded as Tambayan 101.9, is the flagship FM station of the ABS-CBN Corporation in the Philippines.', 'http://stream.rawfm.com.au:8009/listen.pls', 'http://www.tambayan1019.com/', 'Tambayan-1019.png', 0, 0, 0, 1, '2011-11-11', 13615, 0, '2017-01-01 00:00:00', 0), \
('Tech-Nation', '', 'http://stream.rawfm.com.au:8009/listen.pls', '', 'Tech-Nation.png', 0, 0, 0, 1, '2011-11-11', 13616, 0, '2017-01-01 00:00:00', 0), \
('Radio-SLR-1010', '', 'http://netradio.radioslr.dk', 'http://www.radioslr.dk/', 'Radio-SLR-1010.png', 0, 0, 0, 1, '2011-11-11', 13617, 0, '2017-01-01 00:00:00', 0), \
('The-Weekend-Gardener', '', 'http://netradio.radioslr.dk', '', 'The-Weekend-Gardener.png', 0, 0, 0, 1, '2011-11-11', 13618, 0, '2017-01-01 00:00:00', 0), \
('Q100-997', '', 'mms://live.cumulusstreaming.com/WWWQ-FM', 'http://www.allthehitsq100.com/', 'Q100-997.png', 0, 0, 0, 1, '2011-11-11', 13620, 0, '2017-01-01 00:00:00', 0), \
('1027FM', '', 'http://mega5.radioserver.co.uk:8134/', '', '1027FM.png', 0, 0, 0, 1, '2011-11-11', 13621, 0, '2017-01-01 00:00:00', 0), \
('True-Oldies-1063', '', 'http://www.ophanim.net:7320/listen.pls', 'http://www.trueoldies1063.com/', 'True-Oldies-1063.png', 0, 0, 0, 1, '2011-11-11', 13622, 0, '2017-01-01 00:00:00', 0), \
('Kosher-Metal', 'Online Metal Radio - Uncut and Uncensored m/', 'http://208.85.240.102:80/589.64.aac', 'http://loudcaster.com/channels/589-kosher-metal', 'Kosher-Metal.png', 0, 0, 0, 1, '2011-11-11', 13623, 0, '2017-01-01 00:00:00', 0), \
('WPSL-1590', 'WPSL AM-1590 Port St. Lucie, Florida, The Talk of the Treasure Coast.', 'http://208.85.240.102:80/589.64.aac', 'http://www.wpsl.com/', 'WPSL-1590.png', 0, 0, 0, 1, '2011-11-11', 13624, 0, '2017-01-01 00:00:00', 0), \
('Smooth-Radio-Glasgow-1052', '', 'http://streaming.gmgradio.com/smoothradioglasgow.m3u', 'http://www.smoothradioglasgow.co.uk/', 'Smooth-Radio-Glasgow-1052.png', 0, 0, 0, 1, '2011-11-11', 13625, 0, '2017-01-01 00:00:00', 0), \
('KSKK-947', '', 'mms://nick9.surfernetwork.com/KSKK', 'http://www.kkradionetwork.com/', 'KSKK-947.png', 0, 0, 0, 1, '2011-11-11', 13626, 0, '2017-01-01 00:00:00', 0), \
('No-Holds-Barred-Radio', '', 'http://www.newerastreaming.com:8000', 'http://www.nhbradio.com/', 'No-Holds-Barred-Radio.png', 0, 0, 0, 1, '2011-11-11', 13627, 0, '2017-01-01 00:00:00', 0), \
('KPND-953', '', 'http://www.newerastreaming.com:8000', 'http://www.953kpnd.com/', 'KPND-953.png', 0, 0, 0, 1, '2011-11-11', 13628, 0, '2017-01-01 00:00:00', 0), \
('Georgia-Tech-Yellow-Jackets-at-Virginia-Cavaliers-Oct-15-2011', '', 'http://www.newerastreaming.com:8000', '', 'Georgia-Tech-Yellow-Jackets-at-Virginia-Cavaliers-Oct-15-2011.png', 0, 0, 0, 1, '2011-11-11', 13629, 0, '2017-01-01 00:00:00', 0), \
('Mary-Anne-Hobbs', '', 'http://www.newerastreaming.com:8000', '', 'Mary-Anne-Hobbs.png', 0, 0, 0, 1, '2011-11-11', 13630, 0, '2017-01-01 00:00:00', 0), \
('KRCY-FM-967', '', 'http://www.newerastreaming.com:8000', 'http://www.maddog.net/', 'KRCY-FM-967.png', 0, 0, 0, 1, '2011-11-11', 13631, 0, '2017-01-01 00:00:00', 0), \
('Los-Angeles-Kings-Play-by-Play', '', 'http://www.newerastreaming.com:8000', '', 'Los-Angeles-Kings-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 13633, 0, '2017-01-01 00:00:00', 0), \
('The-Front-Row', '', 'http://www.newerastreaming.com:8000', '', 'The-Front-Row.png', 0, 0, 0, 1, '2011-11-11', 13634, 0, '2017-01-01 00:00:00', 0), \
('WGNB-893', '', 'http://209.62.16.60:80/wgnb.mp3', 'http://www.mbn.org/GenMoody/default.asp?sectionid=ccdc3e516b72400bbf827edb0d31da4f', 'WGNB-893.png', 0, 0, 0, 1, '2011-11-11', 13635, 0, '2017-01-01 00:00:00', 0), \
('Get-1025', '', 'http://www.jzaa.com/radio/fileonline/radio102-5.asx', 'http://thisisclick.com/get/', 'Get-1025.png', 0, 0, 0, 1, '2011-11-11', 13636, 0, '2017-01-01 00:00:00', 0), \
('Brian-Mason', '', 'http://www.jzaa.com/radio/fileonline/radio102-5.asx', '', 'Brian-Mason.png', 0, 0, 0, 1, '2011-11-11', 13637, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-70s-Pop', 'Pure retro-pop, with all the 70s hits, one hit wonders, and forgotten guilty pleasures - all the music that used to make listening to the radio really fun and exciting.', 'http://streampoint.radioio.com/streams/20/46f28fc4c4be5/listen.pls', 'http://www.radioio.com/channels/70s/pop', 'RadioIO-70s-Pop.png', 0, 0, 0, 1, '2011-11-11', 13638, 0, '2017-01-01 00:00:00', 0), \
('Praise-1047', '', 'http://streampoint.radioio.com/streams/20/46f28fc4c4be5/listen.pls', 'http://praiserichmond.com/', 'Praise-1047.png', 0, 0, 0, 1, '2011-11-11', 13639, 0, '2017-01-01 00:00:00', 0), \
('Maysa', '', 'http://streampoint.radioio.com/streams/20/46f28fc4c4be5/listen.pls', '', 'Maysa.png', 0, 0, 0, 1, '2011-11-11', 13640, 0, '2017-01-01 00:00:00', 0), \
('MMA-Nation', '', 'http://live-icy.gss.dr.dk:8000/Channel8_HQ.mp3', '', 'MMA-Nation.png', 0, 0, 0, 1, '2011-11-11', 13642, 0, '2017-01-01 00:00:00', 0), \
('Big-Joe-Turner', '', 'http://live-icy.gss.dr.dk:8000/Channel8_HQ.mp3', '', 'Big-Joe-Turner.png', 0, 0, 0, 1, '2011-11-11', 13644, 0, '2017-01-01 00:00:00', 0), \
('Chris-Tomlin', '', 'http://live-icy.gss.dr.dk:8000/Channel8_HQ.mp3', '', 'Chris-Tomlin.png', 0, 0, 0, 1, '2011-11-11', 13645, 0, '2017-01-01 00:00:00', 0), \
('Esperanza-Spalding', '', 'http://live-icy.gss.dr.dk:8000/Channel8_HQ.mp3', '', 'Esperanza-Spalding.png', 0, 0, 1, 1, '2011-11-11', 13646, 0, '2011-10-14 16:07:27', 0), \
('Teen-FM', '', 'http://teenfm.co.uk:8000/stream', 'http://www.teenfm.co.uk/', 'Teen-FM.png', 0, 0, 0, 1, '2011-11-11', 13647, 0, '2017-01-01 00:00:00', 0), \
('WJOB-1230', '', 'http://teenfm.co.uk:8000/stream', 'http://www.wjob1230.com/', 'WJOB-1230.png', 0, 0, 0, 1, '2011-11-11', 13648, 0, '2017-01-01 00:00:00', 0), \
('Antena-Zagreb-897', '', 'http://s7.iqstreaming.com/tunein.php/antenazagrebmp3/playlist.pls', 'http://www.antenazagreb.hr/', 'Antena-Zagreb-897.png', 0, 0, 0, 1, '2011-11-11', 13649, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning', '', 'http://s7.iqstreaming.com/tunein.php/antenazagrebmp3/playlist.pls', '', 'Sunday-Morning.png', 0, 0, 0, 1, '2011-11-11', 13650, 0, '2017-01-01 00:00:00', 0), \
('Henny-Basballe', '', 'http://s7.iqstreaming.com/tunein.php/antenazagrebmp3/playlist.pls', '', 'Henny-Basballe.png', 0, 0, 0, 1, '2011-11-11', 13651, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-1051', '', 'http://s7.iqstreaming.com/tunein.php/antenazagrebmp3/playlist.pls', 'http://www.1051thewolf.com/', 'The-Wolf-1051.png', 0, 0, 0, 1, '2011-11-11', 13652, 0, '2017-01-01 00:00:00', 0), \
('Karma-Love-1048', '', 'http://92.48.118.17:10480/', 'http://www.karmalove.com.tr/', 'Karma-Love-1048.png', 0, 0, 0, 1, '2011-11-11', 13653, 0, '2017-01-01 00:00:00', 0), \
('Let-It-Grow', '', 'http://92.48.118.17:10480/', '', 'Let-It-Grow.png', 0, 0, 0, 1, '2011-11-11', 13654, 0, '2017-01-01 00:00:00', 0), \
('Hip-Hop', '', 'http://92.48.118.17:10480/', '', 'Hip-Hop.png', 0, 0, 0, 1, '2011-11-11', 13655, 0, '2017-01-01 00:00:00', 0), \
('World-Wrestling-ORANGE-(WWO)', '', 'http://92.48.118.17:10480/', '', 'World-Wrestling-ORANGE-(WWO).png', 0, 0, 0, 1, '2011-11-11', 13656, 0, '2017-01-01 00:00:00', 0), \
('Mix-96-965', '', 'http://92.48.118.17:10480/', 'http://mix96tulsa.com/', 'Mix-96-965.png', 0, 0, 0, 1, '2011-11-11', 13657, 0, '2017-01-01 00:00:00', 0), \
('The-Bobcat-935', '', 'mms://nick9.surfernetwork.com/WBBC', 'http://www.bobcatcountryradio.com/', 'The-Bobcat-935.png', 0, 0, 0, 1, '2011-11-11', 13658, 0, '2017-01-01 00:00:00', 0), \
('Fox-Fantasy-Freaks', '', 'mms://nick9.surfernetwork.com/WBBC', '', 'Fox-Fantasy-Freaks.png', 0, 0, 0, 1, '2011-11-11', 13659, 0, '2017-01-01 00:00:00', 0), \
('ROCK-ANTENNE-Erding-Freising-Ebersberg-879', '', 'http://www.rockantenne.de/webradio/channels/rockantennelocal01.aac.pls', 'http://rockantenne.de/', 'ROCK-ANTENNE-Erding-Freising-Ebersberg-879.png', 0, 0, 0, 1, '2011-11-11', 13660, 0, '2017-01-01 00:00:00', 0), \
('Z-1043', '', 'http://www.rockantenne.de/webradio/channels/rockantennelocal01.aac.pls', 'http://www.z1043.com/', 'Z-1043.png', 0, 0, 0, 1, '2011-11-11', 13661, 0, '2017-01-01 00:00:00', 0), \
('The-Pollack-and-Bell-Show', '', 'http://www.rockantenne.de/webradio/channels/rockantennelocal01.aac.pls', '', 'The-Pollack-and-Bell-Show.png', 0, 0, 0, 1, '2011-11-11', 13662, 0, '2017-01-01 00:00:00', 0), \
('Get-the-Led-Out', '', 'http://www.rockantenne.de/webradio/channels/rockantennelocal01.aac.pls', '', 'Get-the-Led-Out.png', 0, 0, 0, 1, '2011-11-11', 13663, 0, '2017-01-01 00:00:00', 0), \
('Aberdeen-Ironbirds-Baseball-Network', '', 'http://www.rockantenne.de/webradio/channels/rockantennelocal01.aac.pls', '', 'Aberdeen-Ironbirds-Baseball-Network.png', 0, 0, 0, 1, '2011-11-11', 13664, 0, '2017-01-01 00:00:00', 0), \
('His-Channel', '', 'http://64.27.10.24:8080/', 'http://www.hischannel.com/', 'His-Channel.png', 0, 0, 0, 1, '2011-11-11', 13665, 0, '2017-01-01 00:00:00', 0), \
('Tennessee-Football-Post-Game', '', 'http://64.27.10.24:8080/', '', 'Tennessee-Football-Post-Game.png', 0, 0, 0, 1, '2011-11-11', 13666, 0, '2017-01-01 00:00:00', 0), \
('Infinite-Eargasm', '', 'http://64.27.10.24:8080/', '', 'Infinite-Eargasm.png', 0, 0, 0, 1, '2011-11-11', 13667, 0, '2017-01-01 00:00:00', 0), \
('Andie-Harpers-Mid-Morning', '', 'http://64.27.10.24:8080/', '', 'Andie-Harpers-Mid-Morning.png', 0, 0, 0, 1, '2011-11-11', 13668, 0, '2017-01-01 00:00:00', 0), \
('Rock-Jam-Punk-SKA', '', 'http://217.73.17.75:8016', 'http://www.rockjamradio.cz/', 'Rock-Jam-Punk-SKA.png', 0, 0, 0, 1, '2011-11-11', 13669, 0, '2017-01-01 00:00:00', 0), \
('JQ99-993', '', 'http://stream2.nwrnetwork.com/WJQK', 'http://www.jq99.com/', 'JQ99-993.png', 0, 0, 0, 1, '2011-11-11', 13670, 0, '2017-01-01 00:00:00', 0), \
('WRHIcom-Football-Stream-5', '', 'http://stream2.nwrnetwork.com/WJQK', 'http://wrhi.com', 'WRHIcom-Football-Stream-5.png', 0, 0, 0, 1, '2011-11-11', 13671, 0, '2017-01-01 00:00:00', 0), \
('B-92-921', '', 'http://stream2.nwrnetwork.com/WJQK', 'http://www.b92fm.com/', 'B-92-921.png', 0, 0, 0, 1, '2011-11-11', 13672, 0, '2017-01-01 00:00:00', 0), \
('Fefe-Dobson', '', 'http://stream2.nwrnetwork.com/WJQK', '', 'Fefe-Dobson.png', 0, 0, 0, 1, '2011-11-11', 13673, 0, '2017-01-01 00:00:00', 0), \
('Camron', '', 'http://stream2.nwrnetwork.com/WJQK', '', 'Camron.png', 0, 0, 0, 1, '2011-11-11', 13674, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-1340', '', 'http://stream2.nwrnetwork.com/WJQK', 'http://www.sportsradio1340.net/', 'Fox-Sports-1340.png', 0, 0, 0, 1, '2011-11-11', 13675, 0, '2017-01-01 00:00:00', 0), \
('99-Jamz-991', '', 'http://stream2.nwrnetwork.com/WJQK', 'http://wedr.com/', '99-Jamz-991.png', 0, 0, 0, 1, '2011-11-11', 13676, 0, '2017-01-01 00:00:00', 0), \
('M2-Jazz-Radio', 'Le Jazz a enfin sa radio.', 'http://100.m2stream.fr:9000/', 'http://www.m2radio.fr/?m2=jazz', 'M2-Jazz-Radio.png', 0, 0, 0, 1, '2011-11-11', 13677, 0, '2017-01-01 00:00:00', 0), \
('931-Jamz', '', 'http://100.m2stream.fr:9000/', 'http://www.madtownjamz.com/', '931-Jamz.png', 0, 0, 0, 1, '2011-11-11', 13678, 0, '2017-01-01 00:00:00', 0), \
('WTTL-1310', '', 'http://100.m2stream.fr:9000/', '', 'WTTL-1310.png', 0, 0, 0, 1, '2011-11-11', 13679, 0, '2017-01-01 00:00:00', 0), \
('Whole-Lotta-Shakin', '', 'http://100.m2stream.fr:9000/', '', 'Whole-Lotta-Shakin.png', 0, 0, 0, 1, '2011-11-11', 13680, 0, '2017-01-01 00:00:00', 0), \
('WILK-1031', '', 'http://100.m2stream.fr:9000/', 'http://www.wilknetwork.com/', 'WILK-1031.png', 0, 0, 0, 1, '2011-11-11', 13681, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Eclectic', 'Always familiar and friendly, but never mainstream, Eclectic features artists who feel like old friends, played alongside the newest adult alternative artists and bands.', 'http://streampoint.radioio.com/streams/219/46f28fc4c4be5/listen.pls', 'http://www.radioio.com/channels/eclectic', 'RadioIO-Eclectic.png', 0, 0, 0, 1, '2011-11-11', 13682, 0, '2017-01-01 00:00:00', 0), \
('Stereo-1340', '', 'http://www.warpradio.com/player/mediaserver.asp?id=6939&t=2&streamRate=', 'http://www.wiry.com/', 'Stereo-1340.png', 0, 0, 0, 1, '2011-11-11', 13683, 0, '2017-01-01 00:00:00', 0), \
('AccuRadio-World-Music', '', 'http://www.accuradio.com/shoutcast/links/world_music.pls', 'http://www.accuradio.com/worldmusic/', 'AccuRadio-World-Music.png', 0, 0, 0, 1, '2011-11-11', 13684, 0, '2017-01-01 00:00:00', 0), \
('WSEN-FM-921', '', 'http://www.accuradio.com/shoutcast/links/world_music.pls', 'http://www.wsenfm.com/', 'WSEN-FM-921.png', 0, 0, 0, 1, '2011-11-11', 13685, 0, '2017-01-01 00:00:00', 0), \
('WNCW-887', '', 'http://audio-mp3.ibiblio.org:8000/wncw-128k', 'http://www.wncw.org/', 'WNCW-887.png', 0, 0, 0, 1, '2011-11-11', 13686, 0, '2017-01-01 00:00:00', 0), \
('BIG-1003', '', 'http://audio-mp3.ibiblio.org:8000/wncw-128k', 'http://www.thebigdc.com/', 'BIG-1003.png', 0, 0, 2, 1, '2011-11-11', 13687, 0, '2011-10-17 14:35:11', 1), \
('KBAQ-895', '', 'http://riosal.ic.llnwd.net/stream/riosal_kbaq', 'http://www.kbaq.org/', 'KBAQ-895.png', 0, 0, 0, 1, '2011-11-11', 13688, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1450', '', 'http://riosal.ic.llnwd.net/stream/riosal_kbaq', 'http://www.wvax.com/', 'ESPN-1450.png', 0, 0, 0, 1, '2011-11-11', 13689, 0, '2017-01-01 00:00:00', 0), \
('1007-WITL', '', 'http://riosal.ic.llnwd.net/stream/riosal_kbaq', 'http://www.witl.com/', '1007-WITL.png', 0, 0, 0, 1, '2011-11-11', 13690, 0, '2017-01-01 00:00:00', 0), \
('KRTN-FM-939', 'KRTN-FM (93.9 FM) is a radio station broadcasting a Variety format. Licensed to Enchanted Air, Inc. in beautiful Raton, New Mexico, USA. The station is committed to providing services to the communiti', 'http://riosal.ic.llnwd.net/stream/riosal_kbaq', '', 'KRTN-FM-939.png', 0, 0, 0, 1, '2011-11-11', 13691, 0, '2017-01-01 00:00:00', 0), \
('KSPN-FM-1031', '', 'http://in.icy2.abacast.com/nrcmountain-kspnfm-32.m3u', 'http://www.kspnradio.com/', 'KSPN-FM-1031.png', 0, 0, 0, 1, '2011-11-11', 13692, 0, '2017-01-01 00:00:00', 0), \
('WSOM-600', '', 'mms://live.cumulusstreaming.com/WSOM-AM', 'http://600wsom.com/', 'WSOM-600.png', 0, 0, 0, 1, '2011-11-11', 13693, 0, '2017-01-01 00:00:00', 0), \
('Carnivore', '', 'mms://live.cumulusstreaming.com/WSOM-AM', '', 'Carnivore.png', 0, 0, 0, 1, '2011-11-11', 13694, 0, '2017-01-01 00:00:00', 0), \
('EU-in-East-Jutland', '', 'mms://live.cumulusstreaming.com/WSOM-AM', '', 'EU-in-East-Jutland.png', 0, 0, 0, 1, '2011-11-11', 13695, 0, '2017-01-01 00:00:00', 0), \
('The-Tony-Kornheiser-Show', '', 'mms://live.cumulusstreaming.com/WSOM-AM', '', 'The-Tony-Kornheiser-Show.png', 0, 0, 0, 1, '2011-11-11', 13696, 0, '2017-01-01 00:00:00', 0), \
('Z93-933', '', 'mms://live.cumulusstreaming.com/WSOM-AM', 'http://www.z93kqz.com/', 'Z93-933.png', 0, 0, 0, 1, '2011-11-11', 13697, 0, '2017-01-01 00:00:00', 0), \
('The-Vibe-1027', '', 'mms://live.cumulusstreaming.com/KBBQ-FM', 'http://www.1027thevibe.com/', 'The-Vibe-1027.png', 0, 0, 0, 1, '2011-11-11', 13698, 0, '2017-01-01 00:00:00', 0), \
('Cory-Weeds', '', 'mms://live.cumulusstreaming.com/KBBQ-FM', '', 'Cory-Weeds.png', 0, 0, 0, 1, '2011-11-11', 13699, 0, '2017-01-01 00:00:00', 0), \
('The-Dock-1041', '104.1 The DOCK is Simcoe County&#39;s Greatest Hits!', 'http://ice3.securenetsystems.net:80/CICZ', 'http://www.1041thedock.com', 'The-Dock-1041.png', 0, 0, 0, 1, '2011-11-11', 13700, 0, '2017-01-01 00:00:00', 0), \
('City-Radio-Pattaya-9025', '', 'http://202.170.122.148:8057', 'http://www.cityradiopattaya.com/', 'City-Radio-Pattaya-9025.png', 0, 0, 0, 1, '2011-11-11', 13701, 0, '2017-01-01 00:00:00', 0), \
('WECK-1230', '', 'mms://nick9.surfernetwork.com/WECK', 'http://www.weck1230.com/', 'WECK-1230.png', 0, 0, 0, 1, '2011-11-11', 13702, 0, '2017-01-01 00:00:00', 0), \
('Joe-Nichols', '', 'mms://nick9.surfernetwork.com/WECK', '', 'Joe-Nichols.png', 0, 0, 0, 1, '2011-11-11', 13703, 0, '2017-01-01 00:00:00', 0), \
('TRFM-995', '', 'mms://nick9.surfernetwork.com/WECK', 'http://www.trfm.com.au/', 'TRFM-995.png', 0, 0, 0, 1, '2011-11-11', 13704, 0, '2017-01-01 00:00:00', 0), \
('rejazz', '', 'mms://nick9.surfernetwork.com/WECK', '', 'rejazz.png', 0, 0, 0, 1, '2011-11-11', 13705, 0, '2017-01-01 00:00:00', 0), \
('WYEC-939', '', 'mms://nick9.surfernetwork.com/WECK', 'http://www.regionaldailynews.com/', 'WYEC-939.png', 0, 0, 0, 1, '2011-11-11', 13706, 0, '2017-01-01 00:00:00', 0), \
('Songs-in-the-Night', '', 'mms://nick9.surfernetwork.com/WECK', '', 'Songs-in-the-Night.png', 0, 0, 0, 1, '2011-11-11', 13707, 0, '2017-01-01 00:00:00', 0), \
('Clear-1017', 'A mix of classic rock artists and today&#39;s singer songwriters.', 'mms://nick9.surfernetwork.com/WECK', 'http://www.clear1017.fm/', 'Clear-1017.png', 0, 0, 0, 1, '2011-11-11', 13708, 0, '2017-01-01 00:00:00', 0), \
('Dionne-Warwick', '', 'mms://nick9.surfernetwork.com/WECK', '', 'Dionne-Warwick.png', 0, 0, 0, 1, '2011-11-11', 13709, 0, '2017-01-01 00:00:00', 0), \
('Thrice', '', 'mms://nick9.surfernetwork.com/WECK', '', 'Thrice.png', 0, 0, 0, 1, '2011-11-11', 13710, 0, '2017-01-01 00:00:00', 0), \
('TRT-FM-933', '', 'mms://95.0.159.133/RADYOFM', 'http://www.trt.net.tr/wwwtrt/anasayfa.aspx', 'TRT-FM-933.png', 0, 0, 0, 1, '2011-11-11', 13711, 0, '2017-01-01 00:00:00', 0), \
('Fly-FM-983', 'At 98.3 FLY-FM we enjoy playing the music that Kingston, Ontario knows and loves. Youll hear your favourites of the 70s, 80s and 90s for the best music variety! 98.3 FLY-FM', 'mms://95.0.159.133/RADYOFM', 'http://www.flyfmkingston.com/', 'Fly-FM-983.png', 0, 0, 0, 1, '2011-11-11', 13712, 0, '2017-01-01 00:00:00', 0), \
('WGRK-FM-1057', '', 'mms://95.0.159.133/RADYOFM', 'http://www.kcountry1057.com/', 'WGRK-FM-1057.png', 0, 0, 0, 1, '2011-11-11', 13713, 0, '2017-01-01 00:00:00', 0), \
('KUVA-1023', 'Tejano music, country music and classic country from UvaldeRadio.com', 'mms://95.0.159.133/RADYOFM', 'http://www.uvalderadio.com/', 'KUVA-1023.png', 0, 0, 0, 1, '2011-11-11', 13714, 0, '2017-01-01 00:00:00', 0), \
('SportsJuice---Everett-Silvertips', '', 'http://media10.sportsjuice.com/sjesilvertips', 'http://sportsjuice.com/', 'SportsJuice---Everett-Silvertips.png', 0, 0, 0, 1, '2011-11-11', 13715, 0, '2017-01-01 00:00:00', 0), \
('WZKY-1580', '', 'http://listen.mediatechusa.com:8080/wzky', 'http://www.1010wspc.com/', 'WZKY-1580.png', 0, 0, 0, 1, '2011-11-11', 13716, 0, '2017-01-01 00:00:00', 0), \
('All-Dance-Radio', '', 'http://listen.radionomy.com/all-dance-radio', 'http://www.radionomy.com/en/radio/all-dance-radio', 'All-Dance-Radio.png', 0, 0, 1, 1, '2011-11-11', 13717, 0, '2011-10-15 20:11:45', 0), \
('WKCR-FM-899', '', 'http://kanga.college.columbia.edu:8000/listen.pls', 'http://www.studentaffairs.columbia.edu/wkcr/', 'WKCR-FM-899.png', 0, 0, 0, 1, '2011-11-11', 13718, 0, '2017-01-01 00:00:00', 0), \
('Bomshel', '', 'http://kanga.college.columbia.edu:8000/listen.pls', '', 'Bomshel.png', 0, 0, 0, 1, '2011-11-11', 13719, 0, '2017-01-01 00:00:00', 0), \
('Farm-Radio-1010', '', 'http://alpha.newerastreaming.com:8016', 'http://www.ksir.com/', 'Farm-Radio-1010.png', 0, 0, 0, 1, '2011-11-11', 13720, 0, '2017-01-01 00:00:00', 0), \
('Z-104-1041', '', 'http://alpha.newerastreaming.com:8016', 'http://www.z104fm.com/', 'Z-104-1041.png', 0, 0, 0, 1, '2011-11-11', 13721, 0, '2017-01-01 00:00:00', 0), \
('KUHF-887', '', 'http://129.7.48.200/KUHF_News_CBR.m3u', 'http://app1.kuhf.org/main.php', 'KUHF-887.png', 0, 0, 0, 1, '2011-11-11', 13722, 0, '2017-01-01 00:00:00', 0), \
('WMAC-940', '', 'mms://live.cumulusstreaming.com/WMAC-AM', 'http://www.wmac-am.com/', 'WMAC-940.png', 0, 0, 0, 1, '2011-11-11', 13723, 0, '2017-01-01 00:00:00', 0), \
('Four80East', '', 'mms://live.cumulusstreaming.com/WMAC-AM', '', 'Four80East.png', 0, 0, 0, 1, '2011-11-11', 13724, 0, '2017-01-01 00:00:00', 0), \
('Comedy104', 'Playing your favorite stand up comedy sketches.', 'http://ando1.cdn.radiostorm.com/comedy.pls', 'http://www.comedy104.com/', 'Comedy104.png', 0, 0, 0, 1, '2011-11-11', 13725, 0, '2017-01-01 00:00:00', 0), \
('CBC-Radio-3', 'Canada is home to one of the hottest indie music scenes on the planet and now you can hear why.', 'http://radio3.cbc.ca/nmcradio/webradio.m3u', 'http://radio3.cbc.ca/', 'CBC-Radio-3.png', 0, 0, 0, 1, '2011-11-11', 13726, 0, '2017-01-01 00:00:00', 0), \
('KTYD-999', '', 'http://radio3.cbc.ca/nmcradio/webradio.m3u', 'http://www.ktyd.com/', 'KTYD-999.png', 0, 0, 0, 1, '2011-11-11', 13727, 0, '2017-01-01 00:00:00', 0), \
('Anime-Nexus', '', 'http://87.98.164.224:8000/', 'http://www.animenexus.net/', 'Anime-Nexus.png', 0, 0, 0, 1, '2011-11-11', 13728, 0, '2017-01-01 00:00:00', 0), \
('WSIC-1400', '', 'http://www.incomservice.net:8000', 'http://www.wsicweb.com', 'WSIC-1400.png', 0, 0, 0, 1, '2011-11-11', 13729, 0, '2017-01-01 00:00:00', 0), \
('SWV', '', 'http://www.incomservice.net:8000', '', 'SWV.png', 0, 0, 0, 1, '2011-11-11', 13730, 0, '2017-01-01 00:00:00', 0), \
('Srgio-Mendes', '', 'http://www.incomservice.net:8000', '', 'Srgio-Mendes.png', 0, 0, 0, 1, '2011-11-11', 13731, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-with-Paul-Smoczyk', '', 'http://www.incomservice.net:8000', '', 'Sunday-Morning-with-Paul-Smoczyk.png', 0, 0, 0, 1, '2011-11-11', 13732, 0, '2017-01-01 00:00:00', 0), \
('WKTU-1035', '', 'http://www.incomservice.net:8000', 'http://www.ktu.com/', 'WKTU-1035.png', 0, 0, 0, 1, '2011-11-11', 13733, 0, '2017-01-01 00:00:00', 0), \
('Slaughter', '', 'http://www.incomservice.net:8000', '', 'Slaughter.png', 0, 0, 0, 1, '2011-11-11', 13734, 0, '2017-01-01 00:00:00', 0), \
('Fun101-FM-1011', 'Todays Hits, Five decades of hits and Classic Rock.', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wtga-fm', 'http://www.fun101fm.com', 'Fun101-FM-1011.png', 0, 0, 0, 1, '2011-11-11', 13735, 0, '2017-01-01 00:00:00', 0), \
('Spanish-Programming', '', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wtga-fm', '', 'Spanish-Programming.png', 0, 0, 0, 1, '2011-11-11', 13736, 0, '2017-01-01 00:00:00', 0), \
('Juice-1072', 'Juice 107.2 is The Local Radio Station for Brighton &amp; Hove, UK. Broadcasting LIVE from North Street', 'http://tx.sharp-stream.com/icecast.php?i=juice1072.mp3', 'http://www.juicebrighton.com/', 'Juice-1072.png', 0, 0, 0, 1, '2011-11-11', 13737, 0, '2017-01-01 00:00:00', 0), \
('Sbastien-Tellier', '', 'http://tx.sharp-stream.com/icecast.php?i=juice1072.mp3', '', 'Sbastien-Tellier.png', 0, 0, 0, 1, '2011-11-11', 13738, 0, '2017-01-01 00:00:00', 0), \
('Jazz-FM-885', '', 'http://www.ksbr.net/ksbrcd.m3u', 'http://www.ksbr.net/', 'Jazz-FM-885.png', 0, 0, 0, 1, '2011-11-11', 13739, 0, '2017-01-01 00:00:00', 0);";



const char *radio_station_setupsql43="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('World-Missionary-Evangelism', '', 'http://www.ksbr.net/ksbrcd.m3u', '', 'World-Missionary-Evangelism.png', 0, 0, 0, 1, '2011-11-11', 13740, 0, '2017-01-01 00:00:00', 0), \
('Bonny-Eagle-vs-Deering-Oct-14-2011', '', 'http://www.ksbr.net/ksbrcd.m3u', '', 'Bonny-Eagle-vs-Deering-Oct-14-2011.png', 0, 0, 0, 1, '2011-11-11', 13741, 0, '2017-01-01 00:00:00', 0), \
('WNRS-1420', '', 'http://www.ksbr.net/ksbrcd.m3u', '', 'WNRS-1420.png', 0, 0, 0, 1, '2011-11-11', 13742, 0, '2017-01-01 00:00:00', 0), \
('FOX-Sports', '', 'http://www.ksbr.net/ksbrcd.m3u', '', 'FOX-Sports.png', 0, 0, 0, 1, '2011-11-11', 13743, 0, '2017-01-01 00:00:00', 0), \
('KOFO-1220', '', 'http://www.ksbr.net/ksbrcd.m3u', 'http://www.kofo.com/', 'KOFO-1220.png', 0, 0, 0, 1, '2011-11-11', 13744, 0, '2017-01-01 00:00:00', 0), \
('DHT', '', 'http://www.ksbr.net/ksbrcd.m3u', '', 'DHT.png', 0, 0, 0, 1, '2011-11-11', 13745, 0, '2017-01-01 00:00:00', 0), \
('WFMF-1025', '', 'http://www.ksbr.net/ksbrcd.m3u', 'http://www.wfmf.com/', 'WFMF-1025.png', 0, 0, 0, 1, '2011-11-11', 13746, 0, '2017-01-01 00:00:00', 0), \
('Melodia-Retro-Kanal', '', 'http://melodiafm.spb.ru/live/128.m3u', 'http://melodiafm.spb.ru/', 'Melodia-Retro-Kanal.png', 0, 0, 0, 1, '2011-11-11', 13747, 0, '2017-01-01 00:00:00', 0), \
('Formiddagen', '', 'http://melodiafm.spb.ru/live/128.m3u', '', 'Formiddagen.png', 0, 0, 0, 1, '2011-11-11', 13748, 0, '2017-01-01 00:00:00', 0), \
('Radio-Alfa-1024', '', 'http://netradio.radioalfa.dk', 'http://www.radioalfa.dk/', 'Radio-Alfa-1024.png', 0, 0, 0, 1, '2011-11-11', 13749, 0, '2017-01-01 00:00:00', 0), \
('Foster-the-People', '', 'http://netradio.radioalfa.dk', '', 'Foster-the-People.png', 0, 0, 0, 1, '2011-11-11', 13750, 0, '2017-01-01 00:00:00', 0), \
('Gold-FM-1043', 'GOLD 104.3 the radio station for Good Times and Great Classic Hits. Make a song request, chat with friends, read your horoscope, play games, win prizes, meet your favourite DJ and heaps more', 'http://icecast.arn.com.au/1043.aac.m3u', 'http://www.gold1043.com.au/', 'Gold-FM-1043.png', 0, 0, 0, 1, '2011-11-11', 13751, 0, '2017-01-01 00:00:00', 0), \
('1010-XL', '', 'http://ice2.securenetsystems.net:80/WJXL', 'http://www.1010xl.com/', '1010-XL.png', 0, 0, 0, 1, '2011-11-11', 13752, 0, '2017-01-01 00:00:00', 0), \
('The-Ticket-900', '', 'mms://live.cumulusstreaming.com/WJLG-AM', 'http://www.900theticket.com/', 'The-Ticket-900.png', 0, 0, 0, 1, '2011-11-11', 13753, 0, '2017-01-01 00:00:00', 0), \
('Dear-and-the-Headlights', '', 'mms://live.cumulusstreaming.com/WJLG-AM', '', 'Dear-and-the-Headlights.png', 0, 0, 0, 1, '2011-11-11', 13754, 0, '2017-01-01 00:00:00', 0), \
('WHEO-1270', 'wheo radio, 1270 am, patrick county, stuart virginia, mountain view communications', 'mms://live.cumulusstreaming.com/WJLG-AM', 'http://www.wheo.info/', 'WHEO-1270.png', 0, 0, 0, 1, '2011-11-11', 13755, 0, '2017-01-01 00:00:00', 0), \
('WNRN-919', '', 'http://broadcast.wnrn.org:8000/wnrn.mp3', 'http://www.wnrn.org/', 'WNRN-919.png', 0, 0, 0, 1, '2011-11-11', 13756, 0, '2017-01-01 00:00:00', 0), \
('My-Top-Ten', '', 'http://broadcast.wnrn.org:8000/wnrn.mp3', '', 'My-Top-Ten.png', 0, 0, 0, 1, '2011-11-11', 13757, 0, '2017-01-01 00:00:00', 0), \
('The-Pale-Fountains', '', 'http://broadcast.wnrn.org:8000/wnrn.mp3', '', 'The-Pale-Fountains.png', 0, 0, 0, 1, '2011-11-11', 13758, 0, '2017-01-01 00:00:00', 0), \
('Kiss-FM-961', '96.1 Kiss Fm is Northern Colorado&#39;s Hit Music Channel with Johnjay and Rich in the Morning and Today&#39;s Hit Music All Day Long!', 'http://broadcast.wnrn.org:8000/wnrn.mp3', 'http://www.kissfmcolorado.com/', 'Kiss-FM-961.png', 0, 0, 0, 1, '2011-11-11', 13759, 0, '2017-01-01 00:00:00', 0), \
('Unistar-Radio-995', '', 'http://unistar.by:8000/', 'http://www.unistar.by/', 'Unistar-Radio-995.png', 0, 0, 0, 1, '2011-11-11', 13760, 0, '2017-01-01 00:00:00', 0), \
('95-7-RB-957', '', 'http://unistar.by:8000/', 'http://www.957rnb.com/', '95-7-RB-957.png', 0, 0, 0, 1, '2011-11-11', 13761, 0, '2017-01-01 00:00:00', 0), \
('Radio-Hollandio---Zeeland', '', 'http://www.hollandio.nl/player.php?editie=ze&player=win', 'http://www.hollandio.nl/', 'Radio-Hollandio---Zeeland.png', 0, 0, 0, 1, '2011-11-11', 13762, 0, '2017-01-01 00:00:00', 0), \
('The-Vines', '', 'http://www.hollandio.nl/player.php?editie=ze&player=win', '', 'The-Vines.png', 0, 0, 0, 1, '2011-11-11', 13763, 0, '2017-01-01 00:00:00', 0), \
('Babybird', '', 'http://www.hollandio.nl/player.php?editie=ze&player=win', '', 'Babybird.png', 0, 0, 0, 1, '2011-11-11', 13764, 0, '2017-01-01 00:00:00', 0), \
('Empire-Wrestling-Radio', '', 'http://www.hollandio.nl/player.php?editie=ze&player=win', '', 'Empire-Wrestling-Radio.png', 0, 0, 0, 1, '2011-11-11', 13765, 0, '2017-01-01 00:00:00', 0), \
('DI-Funky-House', 'A fine selection of funky house music!!', 'http://listen.di.fm/partner_mp3/funkyhouse.pls', 'http://www.di.fm/funkyhouse', 'DI-Funky-House.png', 0, 0, 1, 1, '2011-11-11', 13766, 0, '2011-10-17 23:43:36', 0), \
('Gospel-Greats', '', 'http://listen.di.fm/partner_mp3/funkyhouse.pls', '', 'Gospel-Greats.png', 0, 0, 0, 1, '2011-11-11', 13767, 0, '2017-01-01 00:00:00', 0), \
('Detroit-Public-Radio-1019', 'WDET is a community nonprofit institution, actively involved in the life of metro Detroit.', 'http://141.217.119.35:8000', 'http://www.wdetfm.org/', 'Detroit-Public-Radio-1019.png', 0, 0, 0, 1, '2011-11-11', 13768, 0, '2017-01-01 00:00:00', 0), \
('Rejoice-(Gospel-Music)', '', 'http://141.217.119.35:8000', '', 'Rejoice-(Gospel-Music).png', 0, 0, 0, 1, '2011-11-11', 13769, 0, '2017-01-01 00:00:00', 0), \
('Rugby-League-Show', '', 'http://141.217.119.35:8000', '', 'Rugby-League-Show.png', 0, 0, 0, 1, '2011-11-11', 13770, 0, '2017-01-01 00:00:00', 0), \
('Wow-FM-1053', '', 'http://141.217.119.35:8000', 'http://www.1053wow.com/', 'Wow-FM-1053.png', 0, 0, 0, 1, '2011-11-11', 13771, 0, '2017-01-01 00:00:00', 0), \
('DR P3', '', 'http://live-icy.gss.dr.dk:8000/A/A05H.mp3', 'http://www.dr.dk/p3/', 'DR-P3-939.png', 0, 0, 0, 1, '2011-11-11', 13772, 0, '2017-01-01 00:00:00', 0), \
('Radio-100-1036', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://radio100.dk', 'Radio-100-1036.png', 0, 0, 0, 1, '2011-11-11', 13773, 0, '2017-01-01 00:00:00', 0), \
('Liz-Phair', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'Liz-Phair.png', 0, 0, 0, 1, '2011-11-11', 13774, 0, '2017-01-01 00:00:00', 0), \
('ESPN-730', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://foxsports730.com/', 'ESPN-730.png', 0, 0, 0, 1, '2011-11-11', 13775, 0, '2017-01-01 00:00:00', 0), \
('The-Fall', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'The-Fall.png', 0, 0, 0, 1, '2011-11-11', 13776, 0, '2017-01-01 00:00:00', 0), \
('Your-Money-and-You', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'Your-Money-and-You.png', 0, 0, 0, 1, '2011-11-11', 13777, 0, '2017-01-01 00:00:00', 0), \
('Kitchens-of-Distinction', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'Kitchens-of-Distinction.png', 0, 0, 0, 1, '2011-11-11', 13778, 0, '2017-01-01 00:00:00', 0), \
('Power98-979', 'Power 98 is a Hip-Hop, Soul station based in Charlotte, NC. Power 98 can be heard at radio.com', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.power98fm.com/', 'Power98-979.png', 0, 0, 0, 1, '2011-11-11', 13779, 0, '2017-01-01 00:00:00', 0), \
('The-WYRE-1055', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.wyrefm.com', 'The-WYRE-1055.png', 0, 0, 0, 1, '2011-11-11', 13780, 0, '2017-01-01 00:00:00', 0), \
('The-Phil-Collins-Big-Band', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'The-Phil-Collins-Big-Band.png', 0, 0, 0, 1, '2011-11-11', 13781, 0, '2017-01-01 00:00:00', 0), \
('Miranda-Lambert', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'Miranda-Lambert.png', 0, 0, 0, 1, '2011-11-11', 13782, 0, '2017-01-01 00:00:00', 0), \
('WXJZ-1009', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.wxjz.fm/', 'WXJZ-1009.png', 0, 0, 0, 1, '2011-11-11', 13783, 0, '2017-01-01 00:00:00', 0), \
('KMCD-1570', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.exploreseiowa.com/', 'KMCD-1570.png', 0, 0, 0, 1, '2011-11-11', 13784, 0, '2017-01-01 00:00:00', 0), \
('Club-937', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.club937.com/', 'Club-937.png', 0, 0, 0, 1, '2011-11-11', 13785, 0, '2017-01-01 00:00:00', 0), \
('Cricket', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'Cricket.png', 0, 0, 0, 1, '2011-11-11', 13786, 0, '2017-01-01 00:00:00', 0), \
('News-Talk-820-WBAP', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.wbap.com/', 'News-Talk-820-WBAP.png', 0, 0, 0, 1, '2011-11-11', 13787, 0, '2017-01-01 00:00:00', 0), \
('Bayou-957', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.bayou957.com/', 'Bayou-957.png', 0, 0, 1, 1, '2011-11-11', 13788, 0, '2011-10-14 16:03:04', 0), \
('7-Worlds-Collide', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', '7-Worlds-Collide.png', 0, 0, 0, 1, '2011-11-11', 13789, 0, '2017-01-01 00:00:00', 0), \
('Mitch-Ryder--The-Detroit-Wheels', '', 'http://onair.100fmlive.dk/100fm_live.mp3', '', 'Mitch-Ryder--The-Detroit-Wheels.png', 0, 0, 0, 1, '2011-11-11', 13790, 0, '2017-01-01 00:00:00', 0), \
('WEEI-850', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://www.weei.com/', 'WEEI-850.png', 0, 0, 0, 1, '2011-11-11', 13791, 0, '2017-01-01 00:00:00', 0), \
('Power-991', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', 'http://www.power991fm.com/', 'Power-991.png', 0, 0, 0, 1, '2011-11-11', 13792, 0, '2017-01-01 00:00:00', 0), \
('Hiroshima', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', '', 'Hiroshima.png', 0, 0, 0, 1, '2011-11-11', 13793, 0, '2017-01-01 00:00:00', 0), \
('SymphonyCast', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', '', 'SymphonyCast.png', 0, 0, 0, 1, '2011-11-11', 13794, 0, '2017-01-01 00:00:00', 0), \
('Akon', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', '', 'Akon.png', 0, 0, 0, 1, '2011-11-11', 13795, 0, '2017-01-01 00:00:00', 0), \
('Dance-Gala', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', '', 'Dance-Gala.png', 0, 0, 0, 1, '2011-11-11', 13796, 0, '2017-01-01 00:00:00', 0), \
('La-Kalle-1003', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', 'http://lakalle1003.univision.com/', 'La-Kalle-1003.png', 0, 0, 0, 1, '2011-11-11', 13797, 0, '2017-01-01 00:00:00', 0), \
('Collective-Soul', '', 'http://asx.abacast.com/nnbtricities-kuj-48.pls', '', 'Collective-Soul.png', 0, 0, 0, 1, '2011-11-11', 13798, 0, '2017-01-01 00:00:00', 0), \
('The-Bay-1007', '', 'http://icy2.abacast.com/shamrock-wzbafm-32.m3u', 'http://www.thebayonline.com/', 'The-Bay-1007.png', 0, 0, 0, 1, '2011-11-11', 13799, 0, '2017-01-01 00:00:00', 0), \
('Weekend', '', 'http://icy2.abacast.com/shamrock-wzbafm-32.m3u', '', 'Weekend.png', 0, 0, 0, 1, '2011-11-11', 13800, 0, '2017-01-01 00:00:00', 0), \
('SCC-Radio', '', 'http://67.212.173.250:8680/', 'http://sccradio.yolasite.com/', 'SCC-Radio.png', 0, 0, 0, 1, '2011-11-11', 13801, 0, '2017-01-01 00:00:00', 0), \
('181FM-Trance-Jazz', '', 'http://icyrelay.181.fm/181-trancejazz_128k.mp3', 'http://www.181.fm/', '181FM-Trance-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13802, 0, '2017-01-01 00:00:00', 1), \
('Radio-Cultural-TGN-1005', '', 'http://www.turadionline.com/cast/tunein.php/tgn/playlist.pls', 'http://radiocultural.com/', 'Radio-Cultural-TGN-1005.png', 0, 0, 0, 1, '2011-11-11', 13803, 0, '2017-01-01 00:00:00', 0), \
('KVFC-740', '', 'http://www.turadionline.com/cast/tunein.php/tgn/playlist.pls', 'http://www.kvfcradio.com/', 'KVFC-740.png', 0, 0, 0, 1, '2011-11-11', 13804, 0, '2017-01-01 00:00:00', 0), \
('The-Presets', '', 'http://www.turadionline.com/cast/tunein.php/tgn/playlist.pls', '', 'The-Presets.png', 0, 0, 0, 1, '2011-11-11', 13805, 0, '2017-01-01 00:00:00', 0), \
('Jazz-91-911', '', 'http://www.abacast.com/media/pls/kcsm/kcsm-kcsm-sc64.pls', 'http://www.kcsm.org/', 'Jazz-91-911.png', 0, 0, 0, 1, '2011-11-11', 13806, 0, '2017-01-01 00:00:00', 0), \
('Science360-Radio', 'Science360 Radio features 100+ radio shows and podcasts from the U.S. and around the world, delivering the latest news, in-depth interviews, and lively discussions about all things science, technology', 'http://media.science360.gov/live.m3u', 'http://science360.gov/radio/', 'Science360-Radio.png', 0, 0, 0, 1, '2011-11-11', 13807, 0, '2017-01-01 00:00:00', 0), \
('GotRadio-Reggae', 'Totally Home Grown and great for the soul.', 'http://www.gotradio.com/itunes/Reggae.pls', 'http://www.gotradio.com/', 'GotRadio-Reggae.png', 0, 0, 0, 1, '2011-11-11', 13808, 0, '2017-01-01 00:00:00', 0), \
('WYBCX', '', 'http://relay.wybc.com:8000/x.mp3', 'http://wybcx.com', 'WYBCX.png', 0, 0, 0, 1, '2011-11-11', 13809, 0, '2017-01-01 00:00:00', 0), \
('Two-Episodes-of-Mash', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Two-Episodes-of-Mash.png', 0, 0, 0, 1, '2011-11-11', 13810, 0, '2017-01-01 00:00:00', 0), \
('V1073', '', 'http://relay.wybc.com:8000/x.mp3', 'http://www.1073cleveland.com', 'V1073.png', 0, 0, 0, 1, '2011-11-11', 13811, 0, '2017-01-01 00:00:00', 0), \
('NBA-Sunday', '', 'http://relay.wybc.com:8000/x.mp3', '', 'NBA-Sunday.png', 0, 0, 0, 1, '2011-11-11', 13812, 0, '2017-01-01 00:00:00', 0), \
('Shake-Rattle--Troll', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Shake-Rattle--Troll.png', 0, 0, 0, 1, '2011-11-11', 13813, 0, '2017-01-01 00:00:00', 0), \
('Anita-ODay', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Anita-ODay.png', 0, 0, 0, 1, '2011-11-11', 13814, 0, '2017-01-01 00:00:00', 0), \
('Country-Dreams', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Country-Dreams.png', 0, 0, 0, 1, '2011-11-11', 13815, 0, '2017-01-01 00:00:00', 0), \
('Go-106-FM-1061', '', 'http://relay.wybc.com:8000/x.mp3', 'http://www.go106.com/', 'Go-106-FM-1061.png', 0, 0, 0, 1, '2011-11-11', 13816, 0, '2017-01-01 00:00:00', 0), \
('Kiss-105-108-1056', '', 'http://relay.wybc.com:8000/x.mp3', 'http://www.totalkiss.com/?station=105', 'Kiss-105-108-1056.png', 0, 0, 0, 1, '2011-11-11', 13817, 0, '2017-01-01 00:00:00', 0), \
('The-Mike-Harding-Show', '', 'http://relay.wybc.com:8000/x.mp3', '', 'The-Mike-Harding-Show.png', 0, 0, 0, 1, '2011-11-11', 13818, 0, '2017-01-01 00:00:00', 0), \
('WLTP-910', '', 'http://relay.wybc.com:8000/x.mp3', 'http://www.wltp.com/', 'WLTP-910.png', 0, 0, 0, 1, '2011-11-11', 13819, 0, '2017-01-01 00:00:00', 0), \
('Hot-104-1041', '', 'http://relay.wybc.com:8000/x.mp3', 'http://www.hot1041stl.com/', 'Hot-104-1041.png', 0, 0, 0, 1, '2011-11-11', 13820, 0, '2017-01-01 00:00:00', 0), \
('WKYW-1490', '', 'http://relay.wybc.com:8000/x.mp3', '', 'WKYW-1490.png', 0, 0, 0, 1, '2011-11-11', 13821, 0, '2017-01-01 00:00:00', 0), \
('Vanessa-Amorosi', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Vanessa-Amorosi.png', 0, 0, 0, 1, '2011-11-11', 13822, 0, '2017-01-01 00:00:00', 0), \
('The-Psychedelic-Furs', '', 'http://relay.wybc.com:8000/x.mp3', '', 'The-Psychedelic-Furs.png', 0, 0, 0, 1, '2011-11-11', 13823, 0, '2017-01-01 00:00:00', 0), \
('Pussycat-Dolls', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Pussycat-Dolls.png', 0, 0, 0, 1, '2011-11-11', 13824, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Devotionals', '', 'http://relay.wybc.com:8000/x.mp3', '', 'Sunday-Devotionals.png', 0, 0, 0, 1, '2011-11-11', 13825, 0, '2017-01-01 00:00:00', 0), \
('FM-99-987', '', 'http://relay.wybc.com:8000/x.mp3', 'http://www.fm99.com/', 'FM-99-987.png', 0, 0, 0, 1, '2011-11-11', 13826, 0, '2017-01-01 00:00:00', 0), \
('965-JACK-FM', '96.5 JACK FM broadcasts out of Seattle Washington. The call letters are KJAQ and can be heard on 96.5 FM. They broadcast a format of Music Variety including, Adult, 70s, 80s, 90s, and Rock.', 'http://relay.wybc.com:8000/x.mp3', 'http://www.965jackfm.com/', '965-JACK-FM.png', 0, 0, 0, 1, '2011-11-11', 13827, 0, '2017-01-01 00:00:00', 0), \
('NPR-All-Songs-Considered', 'Eclectic mix of music with a passion for indie rock spiced with 20s jazz, 60s rock folk, electronica and more', 'http://npr.ic.llnwd.net/stream/npr_music2.pls', 'http://www.npr.org/programs/asc/', 'NPR-All-Songs-Considered.png', 0, 0, 0, 1, '2011-11-11', 13828, 0, '2017-01-01 00:00:00', 0), \
('Test-Match-Special', '', 'http://npr.ic.llnwd.net/stream/npr_music2.pls', '', 'Test-Match-Special.png', 0, 0, 0, 1, '2011-11-11', 13829, 0, '2017-01-01 00:00:00', 0), \
('KHSN-1230', '', 'http://npr.ic.llnwd.net/stream/npr_music2.pls', 'http://www.khsn1230.com/', 'KHSN-1230.png', 0, 0, 0, 1, '2011-11-11', 13830, 0, '2017-01-01 00:00:00', 0), \
('The-Ranch-1041', '', 'http://crystalout.surfernetwork.com:8001/KKUS_MP3', 'http://www.theranch.fm/', 'The-Ranch-1041.png', 0, 0, 0, 1, '2011-11-11', 13831, 0, '2017-01-01 00:00:00', 0), \
('WOKA-1310', '', 'http://crystalout.surfernetwork.com:8001/KKUS_MP3', 'http://ladivinamisericorida.webs.com/', 'WOKA-1310.png', 0, 0, 0, 1, '2011-11-11', 13832, 0, '2017-01-01 00:00:00', 0), \
('Billy-Joel', '', 'http://crystalout.surfernetwork.com:8001/KKUS_MP3', '', 'Billy-Joel.png', 0, 0, 0, 1, '2011-11-11', 13833, 0, '2017-01-01 00:00:00', 0), \
('FM-Globo-933', '', 'http://mms.miradio.com.sv/globo/listen.pls', 'http://www.fmglobo.com.sv/globoexitos.htm', 'FM-Globo-933.png', 0, 0, 0, 1, '2011-11-11', 13834, 0, '2017-01-01 00:00:00', 0), \
('NHK-World', 'NHK WORLD is NHKs international broadcast service offering its content via TV, Radio and Internet.', 'http://www.nhk.or.jp/nhkworld/r0/low.asx', 'http://www.nhk.or.jp/nhkworld/', 'NHK-World.png', 0, 0, 0, 1, '2011-11-11', 13835, 0, '2017-01-01 00:00:00', 0), \
('XL-92-3-923', '', 'mms://nick9.surfernetwork.com/WGXL', 'http://www.wgxl.com/', 'XL-92-3-923.png', 0, 0, 0, 1, '2011-11-11', 13836, 0, '2017-01-01 00:00:00', 0), \
('Jim-Lauderdale', '', 'mms://nick9.surfernetwork.com/WGXL', '', 'Jim-Lauderdale.png', 0, 0, 0, 1, '2011-11-11', 13837, 0, '2017-01-01 00:00:00', 0), \
('WONE-FM-975', '', 'mms://nick9.surfernetwork.com/WGXL', 'http://www.wone.net/', 'WONE-FM-975.png', 0, 0, 0, 1, '2011-11-11', 13838, 0, '2017-01-01 00:00:00', 0), \
('Wosch-Die-ENERGY-Abendshow', '', 'mms://nick9.surfernetwork.com/WGXL', '', 'Wosch-Die-ENERGY-Abendshow.png', 0, 0, 0, 1, '2011-11-11', 13839, 0, '2017-01-01 00:00:00', 0), \
('KZEL-FM-961', '', 'mms://live.cumulusstreaming.com/KZEL-FM', 'http://www.96kzel.com/', 'KZEL-FM-961.png', 0, 0, 0, 1, '2011-11-11', 13840, 0, '2017-01-01 00:00:00', 0), \
('KBCN-FM-1043', '', 'mms://live.cumulusstreaming.com/KZEL-FM', 'http://kbcnradio.tripod.com/', 'KBCN-FM-1043.png', 0, 0, 0, 1, '2011-11-11', 13841, 0, '2017-01-01 00:00:00', 0), \
('IRIB-WS-3-Arabic', '', 'mms://77.36.153.27:8083', 'http://worldservice.irib.ir/', 'IRIB-WS-3-Arabic.png', 0, 0, 0, 1, '2011-11-11', 13842, 0, '2017-01-01 00:00:00', 0), \
('His-Name-Is-Jesus', '', 'mms://77.36.153.27:8083', '', 'His-Name-Is-Jesus.png', 0, 0, 0, 1, '2011-11-11', 13843, 0, '2017-01-01 00:00:00', 0), \
('Marvin-Gaye', '', 'mms://77.36.153.27:8083', '', 'Marvin-Gaye.png', 0, 0, 0, 1, '2011-11-11', 13844, 0, '2017-01-01 00:00:00', 0), \
('Sports-Talk-National', '', 'mms://77.36.153.27:8083', '', 'Sports-Talk-National.png', 0, 0, 0, 1, '2011-11-11', 13845, 0, '2017-01-01 00:00:00', 0), \
('Barefoot', '', 'mms://77.36.153.27:8083', '', 'Barefoot.png', 0, 0, 0, 1, '2011-11-11', 13846, 0, '2017-01-01 00:00:00', 0), \
('Cedric-Burnside--Lightnin-Malcolm', '', 'mms://77.36.153.27:8083', '', 'Cedric-Burnside--Lightnin-Malcolm.png', 0, 0, 0, 1, '2011-11-11', 13847, 0, '2017-01-01 00:00:00', 0), \
('Sonic-Youth', '', 'mms://77.36.153.27:8083', '', 'Sonic-Youth.png', 0, 0, 0, 1, '2011-11-11', 13848, 0, '2017-01-01 00:00:00', 0), \
('Via-Audio', '', 'mms://77.36.153.27:8083', '', 'Via-Audio.png', 0, 0, 0, 1, '2011-11-11', 13849, 0, '2017-01-01 00:00:00', 0), \
('XL-1067', '', 'mms://77.36.153.27:8083', 'http://www.xl1067.com/', 'XL-1067.png', 0, 0, 0, 1, '2011-11-11', 13850, 0, '2017-01-01 00:00:00', 0), \
('Blackfoot', '', 'mms://77.36.153.27:8083', '', 'Blackfoot.png', 0, 0, 0, 1, '2011-11-11', 13851, 0, '2017-01-01 00:00:00', 0), \
('WJTN-1240', '', 'mms://77.36.153.27:8083', 'http://www.wjtn.com', 'WJTN-1240.png', 0, 0, 0, 1, '2011-11-11', 13852, 0, '2017-01-01 00:00:00', 0), \
('Prog-Palace-Radio', 'Your complete source of Prog and Power Metal on the net.', 'http://centova2.cpcustomer.com/tunein.php/gstaff02/playlist.pls', 'http://www.progpalaceradio.com/', 'Prog-Palace-Radio.png', 0, 0, 0, 1, '2011-11-11', 13853, 0, '2017-01-01 00:00:00', 0), \
('995-WQYK', '99.5 WQYK is a Country Music station based in St. Petersburg, FL.  99.5 WQYK can be heard at radio.com', 'http://centova2.cpcustomer.com/tunein.php/gstaff02/playlist.pls', 'http://www.wqyk.com/', '995-WQYK.png', 0, 0, 0, 1, '2011-11-11', 13854, 0, '2017-01-01 00:00:00', 0), \
('My-Kinda-Jazz', '', 'http://centova2.cpcustomer.com/tunein.php/gstaff02/playlist.pls', '', 'My-Kinda-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13855, 0, '2017-01-01 00:00:00', 0), \
('CBC-Radio-3-Hip-Hop', '', 'http://webradio.cbcradio3.com/cbc-w8-vcr-128.mp3', 'http://radio3.cbc.ca/', 'CBC-Radio-3-Hip-Hop.png', 0, 0, 0, 1, '2011-11-11', 13856, 0, '2017-01-01 00:00:00', 0), \
('Centinex', '', 'http://webradio.cbcradio3.com/cbc-w8-vcr-128.mp3', '', 'Centinex.png', 0, 0, 0, 1, '2011-11-11', 13857, 0, '2017-01-01 00:00:00', 0), \
('Beirut', '', 'http://webradio.cbcradio3.com/cbc-w8-vcr-128.mp3', '', 'Beirut.png', 0, 0, 0, 1, '2011-11-11', 13858, 0, '2017-01-01 00:00:00', 0), \
('The-Dave-Clark-Five', '', 'http://webradio.cbcradio3.com/cbc-w8-vcr-128.mp3', '', 'The-Dave-Clark-Five.png', 0, 0, 0, 1, '2011-11-11', 13859, 0, '2017-01-01 00:00:00', 0), \
('Ursula-1000', '', 'http://webradio.cbcradio3.com/cbc-w8-vcr-128.mp3', '', 'Ursula-1000.png', 0, 0, 0, 1, '2011-11-11', 13860, 0, '2017-01-01 00:00:00', 0), \
('WLPO-1220', '', 'http://audio.newstrib.com:444/wlpo/', 'http://www.wlpo.net/', 'WLPO-1220.png', 0, 0, 0, 1, '2011-11-11', 13861, 0, '2017-01-01 00:00:00', 0), \
('Naked-Eyes', '', 'http://audio.newstrib.com:444/wlpo/', '', 'Naked-Eyes.png', 0, 0, 0, 1, '2011-11-11', 13862, 0, '2017-01-01 00:00:00', 0), \
('U92-925', '', 'http://audio.newstrib.com:444/wlpo/', 'http://www.u92online.com/', 'U92-925.png', 0, 0, 0, 1, '2011-11-11', 13863, 0, '2017-01-01 00:00:00', 0), \
('Ed-FM-1033', '', 'http://audio.newstrib.com:444/wlpo/', 'http://www.ed.fm/', 'Ed-FM-1033.png', 0, 0, 0, 1, '2011-11-11', 13864, 0, '2017-01-01 00:00:00', 0), \
('Kate-Nash', '', 'http://audio.newstrib.com:444/wlpo/', '', 'Kate-Nash.png', 0, 0, 0, 1, '2011-11-11', 13865, 0, '2017-01-01 00:00:00', 0), \
('Big-983', '', 'http://audio.newstrib.com:444/wlpo/', 'http://www.big983.com', 'Big-983.png', 0, 0, 0, 1, '2011-11-11', 13866, 0, '2017-01-01 00:00:00', 0), \
('The-Ridge-957', '95.7 FM The Ridge is a Classic Hits station serving Rome, Trion, Northwest Georgia and Northeast Alabama. The Ridge plays a variety of your favorite Classic Hits from the 60s, 70s, and 80s.  The Ridge', 'http://72.20.42.91:4020', 'http://www.theridge957.com/theridge.php', 'The-Ridge-957.png', 0, 0, 0, 1, '2011-11-11', 13867, 0, '2017-01-01 00:00:00', 0), \
('WTOP-Live-Interviews', '', 'http://72.20.42.91:4020', '', 'WTOP-Live-Interviews.png', 0, 0, 0, 1, '2011-11-11', 13868, 0, '2017-01-01 00:00:00', 0), \
('1070-The-Fan', 'ESPN affiliate in Indianapolis featuring ESPN and local hosts on Indy and regional sports.', 'http://72.20.42.91:4020', 'http://www.1070thefan.com/', '1070-The-Fan.png', 0, 0, 2, 1, '2011-11-11', 13869, 0, '2011-10-15 20:10:43', 0), \
('K-1047', '', 'http://72.20.42.91:4020', 'http://www.k104online.com/', 'K-1047.png', 0, 0, 0, 1, '2011-11-11', 13870, 0, '2017-01-01 00:00:00', 0), \
('Mountain-Stage', '', 'http://72.20.42.91:4020', '', 'Mountain-Stage.png', 0, 0, 0, 1, '2011-11-11', 13871, 0, '2017-01-01 00:00:00', 0), \
('HateSphere', '', 'http://72.20.42.91:4020', '', 'HateSphere.png', 0, 0, 0, 1, '2011-11-11', 13872, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-921', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', 'http://www.oldiesradioonline.com/home.asp?callsign=WOHF-FM', 'The-Wolf-921.png', 0, 0, 0, 1, '2011-11-11', 13873, 0, '2017-01-01 00:00:00', 0), \
('News-Talk-KOH-AM-780', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', 'http://www.kkoh.com/', 'News-Talk-KOH-AM-780.png', 0, 0, 0, 1, '2011-11-11', 13874, 0, '2017-01-01 00:00:00', 0), \
('Music-Radio-97-971', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', 'http://www.musicradio97.com/', 'Music-Radio-97-971.png', 0, 0, 0, 1, '2011-11-11', 13875, 0, '2017-01-01 00:00:00', 0), \
('Will-Smith', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', '', 'Will-Smith.png', 0, 0, 0, 1, '2011-11-11', 13876, 0, '2017-01-01 00:00:00', 0), \
('Mix-989', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', 'http://www.mix989.com/', 'Mix-989.png', 0, 0, 0, 1, '2011-11-11', 13877, 0, '2017-01-01 00:00:00', 0), \
('UFO', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', '', 'UFO.png', 0, 0, 0, 1, '2011-11-11', 13878, 0, '2017-01-01 00:00:00', 0), \
('The-Handyman-Show', '', 'http://ax1.stream.hostmanager.org/WOHF.asx', '', 'The-Handyman-Show.png', 0, 0, 0, 1, '2011-11-11', 13879, 0, '2017-01-01 00:00:00', 0), \
('Joy-FM-1006', '', 'http://joyfmaac.radyolarburada.com:9100/', 'http://www.joyfm.com.tr/', 'Joy-FM-1006.png', 0, 0, 0, 1, '2011-11-11', 13880, 0, '2017-01-01 00:00:00', 0), \
('Morgonen-Vinyl', '', 'http://joyfmaac.radyolarburada.com:9100/', '', 'Morgonen-Vinyl.png', 0, 0, 0, 1, '2011-11-11', 13881, 0, '2017-01-01 00:00:00', 0), \
('KPLU-885', 'Ranked as one of the most popular public radio stations in the nation, KPLU has been bringing award winning local and National Public Radio news.', 'http://icy1.abacast.com/kplu-newsjazzaac-64', 'http://www.kplu.org/', 'KPLU-885.png', 0, 0, 0, 1, '2011-11-11', 13882, 0, '2017-01-01 00:00:00', 0), \
('Luis-Cali', '', 'http://icy1.abacast.com/kplu-newsjazzaac-64', '', 'Luis-Cali.png', 0, 0, 0, 1, '2011-11-11', 13883, 0, '2017-01-01 00:00:00', 0), \
('WCSX-947', '', 'http://icy1.abacast.com/kplu-newsjazzaac-64', 'http://www.wcsx.com/', 'WCSX-947.png', 0, 0, 0, 1, '2011-11-11', 13884, 0, '2017-01-01 00:00:00', 0), \
('WLKR-1510', 'WLKR is a station as unique as The Firelands.', 'http://icy1.abacast.com/kplu-newsjazzaac-64', 'http://wlkr.northcoastnow.com/', 'WLKR-1510.png', 0, 0, 0, 1, '2011-11-11', 13885, 0, '2017-01-01 00:00:00', 0), \
('WKNU-1063', '', 'http://icy1.abacast.com/kplu-newsjazzaac-64', '', 'WKNU-1063.png', 0, 0, 0, 1, '2011-11-11', 13886, 0, '2017-01-01 00:00:00', 0), \
('Fiesta-1049', '', 'http://serv92.salamancanetwork.com:1977/radiofiesta', 'http://www.fiesta.com.sv/', 'Fiesta-1049.png', 0, 0, 0, 1, '2011-11-11', 13887, 0, '2017-01-01 00:00:00', 0), \
('Breakfast-(Triple-J)', '', 'http://serv92.salamancanetwork.com:1977/radiofiesta', '', '', 0, 0, 0, 1, '2011-11-11', 13888, 0, '2017-01-01 00:00:00', 0), \
('Kicks-1015', '', 'http://serv92.salamancanetwork.com:1977/radiofiesta', 'http://www.kicks1015.com/', 'Kicks-1015.png', 0, 0, 0, 1, '2011-11-11', 13889, 0, '2017-01-01 00:00:00', 0), \
('247-Comedy-Radio', '24/7 Comedy Radio is the world first 24-hour comedy radio network.', 'http://serv92.salamancanetwork.com:1977/radiofiesta', 'http://www.247comedy.com/', '247-Comedy-Radio.png', 0, 0, 0, 1, '2011-11-11', 13890, 0, '2017-01-01 00:00:00', 0), \
('WXPN-885', '', 'http://xpn-web.streamguys.com/xpn96a.asx', 'http://www.xpn.org/', 'WXPN-885.png', 0, 0, 0, 1, '2011-11-11', 13891, 0, '2017-01-01 00:00:00', 0), \
('Z1049', '', 'http://66.208.248.45/kcrz/wmp.asx', 'http://www.z1049.com/', 'Z1049.png', 0, 0, 0, 1, '2011-11-11', 13892, 0, '2017-01-01 00:00:00', 0), \
('The-Herd', '', 'http://66.208.248.45/kcrz/wmp.asx', '', 'The-Herd.png', 0, 0, 0, 1, '2011-11-11', 13893, 0, '2017-01-01 00:00:00', 0), \
('Paloalto', '', 'http://66.208.248.45/kcrz/wmp.asx', '', 'Paloalto.png', 0, 0, 0, 1, '2011-11-11', 13894, 0, '2017-01-01 00:00:00', 0), \
('ORLA-fm', '', 'http://radioorla.co.uk:8000/live', 'http://orla.fm/', 'ORLA-fm.png', 0, 0, 0, 1, '2011-11-11', 13895, 0, '2017-01-01 00:00:00', 0), \
('Rick-Shaw', '', 'http://radioorla.co.uk:8000/live', '', 'Rick-Shaw.png', 0, 0, 0, 1, '2011-11-11', 13896, 0, '2017-01-01 00:00:00', 0), \
('B-921', '', 'http://radioorla.co.uk:8000/live', 'http://b92country.com/', 'B-921.png', 0, 0, 0, 1, '2011-11-11', 13897, 0, '2017-01-01 00:00:00', 0), \
('Cal-Thomas-Commentary', '', 'http://radioorla.co.uk:8000/live', '', 'Cal-Thomas-Commentary.png', 0, 0, 0, 1, '2011-11-11', 13898, 0, '2017-01-01 00:00:00', 0), \
('WQUN-1220', '', 'http://www.streamaudio.com/stations/asx/wqun_am.asx', 'http://www.quinnipiac.edu/x981.xml', 'WQUN-1220.png', 0, 0, 0, 1, '2011-11-11', 13899, 0, '2017-01-01 00:00:00', 0), \
('FX1019', 'FX101.9 Today&#39;s Country playing today&#39;s hit country music. With Denyse Sibley, John Boyd and Julia Killbride!', 'http://english.aliant.net/asx/CHFX.asx', 'http://www.fx1019.ca/', 'FX1019.png', 0, 0, 0, 1, '2011-11-11', 13900, 0, '2017-01-01 00:00:00', 0), \
('JLS', '', 'http://english.aliant.net/asx/CHFX.asx', '', 'JLS.png', 0, 0, 0, 1, '2011-11-11', 13901, 0, '2017-01-01 00:00:00', 0), \
('Mega-955', '', 'http://english.aliant.net/asx/CHFX.asx', 'http://www.mega955.com', 'Mega-955.png', 0, 0, 0, 1, '2011-11-11', 13902, 0, '2017-01-01 00:00:00', 0), \
('Fun-Country-1025', '', 'http://english.aliant.net/asx/CHFX.asx', 'http://www.1025wynr.net/', 'Fun-Country-1025.png', 0, 0, 0, 1, '2011-11-11', 13903, 0, '2017-01-01 00:00:00', 0), \
('Patti-Austin', '', 'http://english.aliant.net/asx/CHFX.asx', '', 'Patti-Austin.png', 0, 0, 0, 1, '2011-11-11', 13904, 0, '2017-01-01 00:00:00', 0), \
('Mix-973', '', 'mms://live.cumulusstreaming.com/KQHN-FM', 'http://www.mixfm973.com/', 'Mix-973.png', 0, 0, 0, 1, '2011-11-11', 13905, 0, '2017-01-01 00:00:00', 0), \
('1067-The-Fan', 'Sportsradio 106.7 The Fan is the new radio station for D.C. sports fans.', 'mms://live.cumulusstreaming.com/KQHN-FM', 'http://1067thefandc.com', '1067-The-Fan.png', 0, 0, 0, 1, '2011-11-11', 13906, 0, '2017-01-01 00:00:00', 0), \
('ZaycevFM-RnB', '', 'http://www.zaycev.fm:9002/rnb/ZaycevFM(256)', 'http://www.zaycev.fm', 'ZaycevFM-RnB.png', 0, 0, 0, 1, '2011-11-11', 13907, 0, '2017-01-01 00:00:00', 0), \
('KOZZ-FM-1057', '', 'http://stream.groovefm.de:10028/', 'http://www.kozzradio.com/', 'KOZZ-FM-1057.png', 0, 0, 0, 1, '2011-11-11', 13909, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1040', 'Tampa Bay&#39;s Sports Leader', 'mms://1.uni1.sa.streamaudio.com/WHBO_AM', 'http://www.espn1040.com/', 'ESPN-Radio-1040.png', 0, 0, 0, 1, '2011-11-11', 13910, 0, '2017-01-01 00:00:00', 0), \
('WHIP-1350', '', 'http://67.159.45.87:8204', 'http://www.carolinascene.com/w/whip/', 'WHIP-1350.png', 0, 0, 0, 1, '2011-11-11', 13911, 0, '2017-01-01 00:00:00', 0), \
('The-Tommy-Mac-Show', '', 'http://67.159.45.87:8204', '', 'The-Tommy-Mac-Show.png', 0, 0, 0, 1, '2011-11-11', 13912, 0, '2017-01-01 00:00:00', 0), \
('Central-Avenue-Church', '', 'http://67.159.45.87:8204', '', 'Central-Avenue-Church.png', 0, 0, 0, 1, '2011-11-11', 13913, 0, '2017-01-01 00:00:00', 0), \
('Highway-40-Country', 'Highway 40 Country provides a fun environment for all of our listeners, we invite you to contact us to put in a live request', 'http://64.64.3.161:8000/', 'http://www.highway40country.com', 'Highway-40-Country.png', 0, 0, 0, 1, '2011-11-11', 13914, 0, '2017-01-01 00:00:00', 0), \
('WKXL-1450', '', 'http://wkxl.serverroom.us:7516', 'http://wkxl1450.com/', 'WKXL-1450.png', 0, 0, 0, 1, '2011-11-11', 13915, 0, '2017-01-01 00:00:00', 0), \
('The-Polish-Program', '', 'http://wkxl.serverroom.us:7516', '', 'The-Polish-Program.png', 0, 0, 0, 1, '2011-11-11', 13916, 0, '2017-01-01 00:00:00', 0), \
('WNEG-630', '', 'http://wkxl.serverroom.us:7516', 'http://www.wnegradio.com/', 'WNEG-630.png', 0, 0, 0, 1, '2011-11-11', 13917, 0, '2017-01-01 00:00:00', 0), \
('HardRadio', '', 'http://www.hardradio.com/streaming/iphone.m3u', 'http://www.hardradio.com/', 'HardRadio.png', 0, 0, 0, 1, '2011-11-11', 13918, 0, '2017-01-01 00:00:00', 0), \
('Digital-1015', '', 'http://wmc1.den.liquidcompass.net/XHAVOFM', 'http://www.clubdigital1015.com/', 'Digital-1015.png', 0, 0, 0, 1, '2011-11-11', 13919, 0, '2017-01-01 00:00:00', 0), \
('The-Sports-Hog-1031', '', 'http://wmc1.den.liquidcompass.net/XHAVOFM', 'http://www.sportshog1031.com/', 'The-Sports-Hog-1031.png', 0, 0, 0, 1, '2011-11-11', 13920, 0, '2017-01-01 00:00:00', 0), \
('The-Backporch', '', 'http://wmc1.den.liquidcompass.net/XHAVOFM', '', 'The-Backporch.png', 0, 0, 0, 1, '2011-11-11', 13921, 0, '2017-01-01 00:00:00', 0), \
('Classic-Country-Gospel-Show', '', 'http://wmc1.den.liquidcompass.net/XHAVOFM', '', 'Classic-Country-Gospel-Show.png', 0, 0, 0, 1, '2011-11-11', 13922, 0, '2017-01-01 00:00:00', 0), \
('VI-Radio', '', 'http://82.201.100.10:8000/WEB12', 'http://www.vi.nl/', 'VI-Radio.png', 0, 0, 0, 1, '2011-11-11', 13923, 0, '2017-01-01 00:00:00', 0), \
('BBC-Radio-1-Scotland-995', '', 'http://www.bbc.co.uk/radio/listen/live/r1.asx?r=scotland', 'http://www.bbc.co.uk/radio1/', 'BBC-Radio-1-Scotland-995.png', 0, 0, 0, 1, '2011-11-11', 13924, 0, '2017-01-01 00:00:00', 0), \
('Ludacris', '', 'http://www.bbc.co.uk/radio/listen/live/r1.asx?r=scotland', '', 'Ludacris.png', 0, 0, 0, 1, '2011-11-11', 13925, 0, '2017-01-01 00:00:00', 0), \
('Simple-Minds', '', 'http://www.bbc.co.uk/radio/listen/live/r1.asx?r=scotland', '', 'Simple-Minds.png', 0, 0, 0, 1, '2011-11-11', 13926, 0, '2017-01-01 00:00:00', 0), \
('AmpedFM-Movie-Magic', 'Amped FM Radio Network has eight internet radio stations. All stations broadcast at 128 kbps for great sound quality. We now have over 500,000 listeners per month.', 'http://www.reliastream.com/cast/tunein.php/ampedmovie/playlist.pls', 'http://www.ampedfm.com/', 'AmpedFM-Movie-Magic.png', 0, 0, 0, 1, '2011-11-11', 13927, 0, '2017-01-01 00:00:00', 0), \
('ESPN-700', '', 'http://www.warpradio.com/player/mediaserver.asp?id=10215&t=2&streamRate=', 'http://www.kall700sports.com/', 'ESPN-700.png', 0, 0, 0, 1, '2011-11-11', 13928, 0, '2017-01-01 00:00:00', 0), \
('Sports-Writers-on-the-Radio', '', 'http://www.warpradio.com/player/mediaserver.asp?id=10215&t=2&streamRate=', '', 'Sports-Writers-on-the-Radio.png', 0, 0, 0, 1, '2011-11-11', 13929, 0, '2017-01-01 00:00:00', 0), \
('Taproot', '', 'http://www.warpradio.com/player/mediaserver.asp?id=10215&t=2&streamRate=', '', 'Taproot.png', 0, 0, 0, 1, '2011-11-11', 13930, 0, '2017-01-01 00:00:00', 0), \
('Planet-Pootwaddle', 'Planet Pootwaddle is an extensive collection of both familiar and eclectic music coupled with a healthy dose of 12 year old boy humor', 'http://war.str3am.com:7200', 'http://www.planetpootwaddle.com/', 'Planet-Pootwaddle.png', 0, 0, 0, 1, '2011-11-11', 13931, 0, '2017-01-01 00:00:00', 0), \
('Temple-of-Restoration', '', 'http://war.str3am.com:7200', '', 'Temple-of-Restoration.png', 0, 0, 0, 1, '2011-11-11', 13932, 0, '2017-01-01 00:00:00', 0), \
('Jincheng-Traffic--Health-Radio-935', '', 'mms://124.164.234.179/live5', 'http://www.jcbctv.com/video/live.aspx?channel=5', 'Jincheng-Traffic--Health-Radio-935.png', 0, 0, 0, 1, '2011-11-11', 13933, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-1400', '', 'mms://124.164.234.179/live5', 'http://www.wcoh.com/', 'Fox-Sports-1400.png', 0, 0, 0, 1, '2011-11-11', 13934, 0, '2017-01-01 00:00:00', 0), \
('The-O2-Evening-Show', '', 'mms://124.164.234.179/live5', '', 'The-O2-Evening-Show.png', 0, 0, 0, 1, '2011-11-11', 13935, 0, '2017-01-01 00:00:00', 0), \
('WKQQ-1001', '', 'mms://124.164.234.179/live5', 'http://www.wkqq.com/', 'WKQQ-1001.png', 0, 0, 0, 1, '2011-11-11', 13936, 0, '2017-01-01 00:00:00', 0), \
('Die-ZWEI-ab-2', '', 'mms://124.164.234.179/live5', '', 'Die-ZWEI-ab-2.png', 0, 0, 0, 1, '2011-11-11', 13937, 0, '2017-01-01 00:00:00', 0), \
('Boxeo', '', 'mms://124.164.234.179/live5', '', 'Boxeo.png', 0, 0, 0, 1, '2011-11-11', 13938, 0, '2017-01-01 00:00:00', 0), \
('9-Country-999', '', 'mms://124.164.234.179/live5', 'http://www.kboz.com/', '9-Country-999.png', 0, 0, 0, 1, '2011-11-11', 13939, 0, '2017-01-01 00:00:00', 0), \
('Noctam-Blues-Jazz', 'The Jazz musician&#39;s Radio - biographies de Musiciens - Chroniques - Concerts Jazz...', 'http://listen.radionomy.com/noctamblues', 'http://www.noctamblues.com/', 'Noctam-Blues-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13940, 0, '2017-01-01 00:00:00', 0), \
('Gardening-Naturally', '', 'http://listen.radionomy.com/noctamblues', '', 'Gardening-Naturally.png', 0, 0, 0, 1, '2011-11-11', 13941, 0, '2017-01-01 00:00:00', 0), \
('Mix-106-1059', '', 'http://listen.radionomy.com/noctamblues', 'http://www.mix106radio.com/', 'Mix-106-1059.png', 0, 0, 0, 1, '2011-11-11', 13942, 0, '2017-01-01 00:00:00', 0), \
('The-Grenade-1430', '', 'http://asx.abacast.com/kanza-kanza2-32.asx', 'http://www.kaolradio.com', 'The-Grenade-1430.png', 0, 0, 0, 1, '2011-11-11', 13943, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1300', '', 'http://asx.abacast.com/kanza-kanza2-32.asx', 'http://www.espnradio1300.com/', 'ESPN-Radio-1300.png', 0, 0, 0, 1, '2011-11-11', 13944, 0, '2017-01-01 00:00:00', 0), \
('Keri-Hilson', '', 'http://asx.abacast.com/kanza-kanza2-32.asx', '', 'Keri-Hilson.png', 0, 0, 0, 1, '2011-11-11', 13945, 0, '2017-01-01 00:00:00', 0), \
('Old-School-Radio', 'Les classiques du funk,dela soul et du hip hop en continu sur Old Schol radio !', 'http://listen.radionomy.com/oldschool', 'http://www.oldschool-radio.com/', 'Old-School-Radio.png', 0, 0, 0, 1, '2011-11-11', 13946, 0, '2017-01-01 00:00:00', 0), \
('WJOI-1230', '', 'http://listen.radionomy.com/oldschool', 'http://www.1230wjoi.com', 'WJOI-1230.png', 0, 0, 0, 1, '2011-11-11', 13947, 0, '2017-01-01 00:00:00', 0), \
('Toadies', '', 'http://listen.radionomy.com/oldschool', '', 'Toadies.png', 0, 0, 0, 1, '2011-11-11', 13948, 0, '2017-01-01 00:00:00', 0), \
('the-EDGE-1021', 'The Edge is one of the most powerful and influential new rock radio stations. Based in Toronto, Canada.', 'http://listen.radionomy.com/oldschool', 'http://www.edge.ca/', 'the-EDGE-1021.png', 0, 0, 0, 1, '2011-11-11', 13949, 0, '2017-01-01 00:00:00', 0), \
('Majic-Jazz', '', 'http://listen.radionomy.com/oldschool', '', 'Majic-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13950, 0, '2017-01-01 00:00:00', 0), \
('Original-106-Aberdeen-1068', 'Launched on October 27th 2007 Original 106 is the local station for the North East of Scotland.  Playing the best mix of music from the 60&#39;s through to today and the only station offering locally ', 'http://tx.sharp-stream.com/icecast.php?i=original106.mp3', 'http://www.originalfm.com/', 'Original-106-Aberdeen-1068.png', 0, 0, 0, 1, '2011-11-11', 13951, 0, '2017-01-01 00:00:00', 0), \
('923-NOW', 'Welcome to New Yorks new hit music channel,92.3 Now.', 'http://tx.sharp-stream.com/icecast.php?i=original106.mp3', 'http://www.923now.com', '923-NOW.png', 0, 0, 2, 1, '2011-11-11', 13952, 0, '2011-10-14 15:57:22', 0), \
('ESPN-SportsCenter', '', 'http://live-icy.gss.dr.dk:8000/Channel16_HQ.mp3', '', 'ESPN-SportsCenter.png', 0, 0, 0, 1, '2011-11-11', 13954, 0, '2017-01-01 00:00:00', 0), \
('Latinos-FM-1017', '', 'http://ns32380.ovh.net:6601/LISTEN.mp3', 'http://www.latinos.fm/', 'Latinos-FM-1017.png', 0, 0, 0, 1, '2011-11-11', 13955, 0, '2017-01-01 00:00:00', 0), \
('KRUZ-975', '', 'mms://live.cumulusstreaming.com/KRUZ-FM', 'http://www.kruz.com/', 'KRUZ-975.png', 0, 0, 0, 1, '2011-11-11', 13957, 0, '2017-01-01 00:00:00', 0), \
('The-Good-Oil', '', 'mms://live.cumulusstreaming.com/KRUZ-FM', '', 'The-Good-Oil.png', 0, 0, 0, 1, '2011-11-11', 13958, 0, '2017-01-01 00:00:00', 0), \
('WKJC-1047', 'The voice of Northern Michigan.', 'http://ice1.securenetsystems.net:80/WKJCM', 'http://www.wkjc.com/', 'WKJC-1047.png', 0, 0, 0, 1, '2011-11-11', 13959, 0, '2017-01-01 00:00:00', 0), \
('WREL-1450', '', 'http://ice1.securenetsystems.net:80/WKJCM', 'http://www.wrel.com/', 'WREL-1450.png', 0, 0, 0, 1, '2011-11-11', 13960, 0, '2017-01-01 00:00:00', 0), \
('NHL-Play-by-Play', 'The puck drops every day on the world&#39;s fastest sport on the world&#39;s first hockey channel.', 'http://ice1.securenetsystems.net:80/WKJCM', 'http://www.xmradio.com/', 'NHL-Play-by-Play.png', 0, 0, 0, 1, '2011-11-11', 13961, 0, '2017-01-01 00:00:00', 0), \
('Explosions-in-the-Sky', '', 'http://ice1.securenetsystems.net:80/WKJCM', '', 'Explosions-in-the-Sky.png', 0, 0, 0, 1, '2011-11-11', 13962, 0, '2017-01-01 00:00:00', 0), \
('KESM-1580', '', 'http://ice1.securenetsystems.net:80/WKJCM', 'http://www.kesmradio.com/index.html', 'KESM-1580.png', 0, 0, 0, 1, '2011-11-11', 13963, 0, '2017-01-01 00:00:00', 0), \
('KQMG-1220', '', 'http://ice1.securenetsystems.net:80/WKJCM', 'http://www.kqmg.org/', 'KQMG-1220.png', 0, 0, 0, 1, '2011-11-11', 13964, 0, '2017-01-01 00:00:00', 0), \
('Anita-Wardell', '', 'http://ice1.securenetsystems.net:80/WKJCM', '', 'Anita-Wardell.png', 0, 0, 0, 1, '2011-11-11', 13965, 0, '2017-01-01 00:00:00', 0), \
('Ken-Bruce', '', 'http://ice1.securenetsystems.net:80/WKJCM', '', 'Ken-Bruce.png', 0, 0, 0, 1, '2011-11-11', 13966, 0, '2017-01-01 00:00:00', 0), \
('Drowning-Pool', '', 'http://ice1.securenetsystems.net:80/WKJCM', '', 'Drowning-Pool.png', 0, 0, 0, 1, '2011-11-11', 13967, 0, '2017-01-01 00:00:00', 0), \
('The-Gospel-Explosion', '', 'http://ice1.securenetsystems.net:80/WKJCM', '', 'The-Gospel-Explosion.png', 0, 0, 0, 1, '2011-11-11', 13968, 0, '2017-01-01 00:00:00', 0), \
('The-Countdown-Kids', '', 'http://ice1.securenetsystems.net:80/WKJCM', '', 'The-Countdown-Kids.png', 0, 0, 0, 1, '2011-11-11', 13969, 0, '2017-01-01 00:00:00', 0), \
('Rewind-949', '', 'http://wmc1.den.liquidcompass.net/WREWFM', 'http://www.rewind949.com/', 'Rewind-949.png', 0, 0, 0, 1, '2011-11-11', 13970, 0, '2017-01-01 00:00:00', 0), \
('Johnny-Hates-Jazz', '', 'http://wmc1.den.liquidcompass.net/WREWFM', '', 'Johnny-Hates-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13971, 0, '2017-01-01 00:00:00', 0), \
('WATN-1240', '', 'mms://streamer2.securenetsystems.net/WATN', 'http://www.gisco.net/watn/', 'WATN-1240.png', 0, 0, 0, 1, '2011-11-11', 13972, 0, '2017-01-01 00:00:00', 0), \
('KWBG-1590', '', 'http://216.248.117.244:8080/', 'http://www.kwbg.com/', 'KWBG-1590.png', 0, 0, 0, 1, '2011-11-11', 13973, 0, '2017-01-01 00:00:00', 0), \
('SKYFM-A-Beatles-Tribute', 'Beatles Hits, Remakes, Tributes!!!', 'http://listen.sky.fm/partner_mp3/beatles.pls', 'http://www.sky.fm/beatles', 'SKYFM-A-Beatles-Tribute.png', 0, 0, 0, 1, '2011-11-11', 13974, 0, '2017-01-01 00:00:00', 0), \
('On-Demand-Weekend', '', 'http://listen.sky.fm/partner_mp3/beatles.pls', '', 'On-Demand-Weekend.png', 0, 0, 0, 1, '2011-11-11', 13975, 0, '2017-01-01 00:00:00', 0), \
('Radio-538-Hitzone', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.radio538.nl/', 'Radio-538-Hitzone.png', 0, 0, 0, 1, '2011-11-11', 13976, 0, '2017-01-01 00:00:00', 0), \
('Florida-Marlins-vs-Angels', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', '', 'Florida-Marlins-vs-Angels.png', 0, 0, 0, 1, '2011-11-11', 13977, 0, '2017-01-01 00:00:00', 0), \
('Star-99-995', 'Music and a mix of sports and news all day long.', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.dakotaradiogroup.com/', 'Star-99-995.png', 0, 0, 0, 1, '2011-11-11', 13978, 0, '2017-01-01 00:00:00', 0), \
('Russel-Brand', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', '', 'Russel-Brand.png', 0, 0, 0, 1, '2011-11-11', 13979, 0, '2017-01-01 00:00:00', 0), \
('KOZN-1620', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.1620thezone.com/', 'KOZN-1620.png', 0, 0, 0, 1, '2011-11-11', 13980, 0, '2017-01-01 00:00:00', 0), \
('The-Best-of-Neal-Boortz', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', '', 'The-Best-of-Neal-Boortz.png', 0, 0, 0, 1, '2011-11-11', 13981, 0, '2017-01-01 00:00:00', 0), \
('Radio-1045', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.radio1045.com/', 'Radio-1045.png', 0, 0, 0, 1, '2011-11-11', 13982, 0, '2017-01-01 00:00:00', 0), \
('The-Special-AKA', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', '', 'The-Special-AKA.png', 0, 0, 0, 1, '2011-11-11', 13983, 0, '2017-01-01 00:00:00', 0), \
('KKTS-FM-993', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.kktyonline.com/', 'KKTS-FM-993.png', 0, 0, 0, 1, '2011-11-11', 13984, 0, '2017-01-01 00:00:00', 0), \
('Minnesota-HomeTalk', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', '', 'Minnesota-HomeTalk.png', 0, 0, 0, 1, '2011-11-11', 13985, 0, '2017-01-01 00:00:00', 0), \
('WTGM-960', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.960thesportsanimal.com/', 'WTGM-960.png', 0, 0, 0, 1, '2011-11-11', 13986, 0, '2017-01-01 00:00:00', 0), \
('WRCO-1450', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', 'http://www.wrco.com/', 'WRCO-1450.png', 0, 0, 0, 1, '2011-11-11', 13987, 0, '2017-01-01 00:00:00', 0), \
('Metric', '', 'http://82.201.100.23/WEB11-538Hitzone.m3u', '', 'Metric.png', 0, 0, 0, 1, '2011-11-11', 13988, 0, '2017-01-01 00:00:00', 0), \
('MAIN-FM-1035', '', 'http://main-fm.org/stream/high.m3u', 'http://www.wpvm.org/', 'MAIN-FM-1035.png', 0, 0, 0, 1, '2011-11-11', 13989, 0, '2017-01-01 00:00:00', 0), \
('Majic-102-1021', '', 'http://main-fm.org/stream/high.m3u', 'http://myhoustonmajic.com/', 'Majic-102-1021.png', 0, 0, 0, 1, '2011-11-11', 13990, 0, '2017-01-01 00:00:00', 0), \
('MSA-Sports-PIHL-Hockey-Channel-3', '', 'http://msasportsnetwork.com/asx.asp?id=197534type=a', 'http://www.msasports.net', 'MSA-Sports-PIHL-Hockey-Channel-3.png', 0, 0, 0, 1, '2011-11-11', 13991, 0, '2017-01-01 00:00:00', 0), \
('Sports-Overnight-America', '', 'http://msasportsnetwork.com/asx.asp?id=197534type=a', '', 'Sports-Overnight-America.png', 0, 0, 0, 1, '2011-11-11', 13992, 0, '2017-01-01 00:00:00', 0), \
('Townhallcom-Weekend-Journal', '', 'http://msasportsnetwork.com/asx.asp?id=197534type=a', '', 'Townhallcom-Weekend-Journal.png', 0, 0, 0, 1, '2011-11-11', 13993, 0, '2017-01-01 00:00:00', 0), \
('Nicole-Scherzinger', '', 'http://msasportsnetwork.com/asx.asp?id=197534type=a', '', 'Nicole-Scherzinger.png', 0, 0, 0, 1, '2011-11-11', 13994, 0, '2017-01-01 00:00:00', 0), \
('RSR-La-Premire-910', '', 'http://stream.srg-ssr.ch/la-1ere/aacp_64.m3u', 'http://www.rsr.ch/', 'RSR-La-Premire-910.png', 0, 0, 0, 1, '2011-11-11', 13995, 0, '2017-01-01 00:00:00', 0), \
('wunschradiofm', 'wunschradio.fm is one of the most popular radio stations in Germany. Listeners are able to request music 24/7.', 'http://80.237.159.58:8080/', 'http://www.wunschradio.de/', 'wunschradiofm.png', 0, 0, 0, 1, '2011-11-11', 13996, 0, '2017-01-01 00:00:00', 0), \
('B-103-5-1035', '', 'http://80.237.159.58:8080/', 'http://www.kzrb103five.com', 'B-103-5-1035.png', 0, 0, 0, 1, '2011-11-11', 13997, 0, '2017-01-01 00:00:00', 0), \
('Trip-Shakespeare', '', 'http://80.237.159.58:8080/', '', 'Trip-Shakespeare.png', 0, 0, 0, 1, '2011-11-11', 13998, 0, '2017-01-01 00:00:00', 0), \
('Fox-News-Radio-1310', '', 'http://80.237.159.58:8080/', 'http://www.klixam.com/', 'Fox-News-Radio-1310.png', 0, 0, 0, 1, '2011-11-11', 13999, 0, '2017-01-01 00:00:00', 0), \
('HeadSpace', '', 'http://80.237.159.58:8080/', '', 'HeadSpace.png', 0, 0, 0, 1, '2011-11-11', 14000, 0, '2017-01-01 00:00:00', 0), \
('Stones-Live', 'The 24/7 internet radio station for fans of Maidstone United Football (Soccer) Club in England. With LIVE commentaries and magazine shows (LIVE and recorded). We broadcast 24 hours a day, 7 days a wee', 'http://shoutcast.internet-radio.org.uk/tunein.php/stoneslive/playlist.pls', 'http://www.stoneslive.co.uk/', 'Stones-Live.png', 0, 0, 0, 1, '2011-11-11', 14001, 0, '2017-01-01 00:00:00', 0), \
('The-Food-Dude-Show', '', 'http://shoutcast.internet-radio.org.uk/tunein.php/stoneslive/playlist.pls', '', 'The-Food-Dude-Show.png', 0, 0, 0, 1, '2011-11-11', 14002, 0, '2017-01-01 00:00:00', 0), \
('KKAN-1490', 'Today&#39;s Hot Country Plus Classic Rock N More!', 'http://65.167.142.251:8000/stream.mp3', 'http://www.kkankqma.com/', 'KKAN-1490.png', 0, 0, 0, 1, '2011-11-11', 14003, 0, '2017-01-01 00:00:00', 0), \
('SportsJuice---Saanich-Jr-Braves', '', 'http://media10.sportsjuice.com/sjsaanichjr', 'http://sportsjuice.com/', 'SportsJuice---Saanich-Jr-Braves.png', 0, 0, 0, 1, '2011-11-11', 14004, 0, '2017-01-01 00:00:00', 0);";




const char *radio_station_setupsql44="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('KSL-News-Radio-1160', '', 'http://stream.ksl.com/kslradio.pls', 'http://www.ksl.com/index.php?nid=16', 'KSL-News-Radio-1160.png', 0, 0, 0, 1, '2011-11-11', 14005, 0, '2017-01-01 00:00:00', 0), \
('Miami-Dolphins-at-New-York-Jets-Oct-17-2011', '', 'http://stream.ksl.com/kslradio.pls', '', '', 0, 0, 0, 1, '2011-11-11', 14006, 0, '2017-01-01 00:00:00', 0), \
('Anaheim-Ducks-at-San-Jose-Sharks-Oct-17-2011', '', 'http://stream.ksl.com/kslradio.pls', '', '', 0, 0, 0, 1, '2011-11-11', 14007, 0, '2017-01-01 00:00:00', 0), \
('Bathory', '', 'http://stream.ksl.com/kslradio.pls', '', 'Bathory.png', 0, 0, 0, 1, '2011-11-11', 14010, 0, '2017-01-01 00:00:00', 0), \
('Poder-1110', '', 'http://stream.ksl.com/kslradio.pls', 'http://www.poder1110.com/', 'Poder-1110.png', 0, 0, 0, 1, '2011-11-11', 14011, 0, '2017-01-01 00:00:00', 0), \
('921', '70s Country Hits', 'http://stream.ksl.com/kslradio.pls', 'http://www.ilove921.com', '921.png', 0, 0, 0, 1, '2011-11-11', 14012, 0, '2017-01-01 00:00:00', 0), \
('The-Eagle-1015', '', 'http://stream.ksl.com/kslradio.pls', 'http://www.1015theeagle.com/', 'The-Eagle-1015.png', 0, 0, 0, 1, '2011-11-11', 14013, 0, '2017-01-01 00:00:00', 0), \
('Radio-C-Dance', '', 'http://streamserver.c-dance.com:8320/', 'http://www.c-dance.com/', 'Radio-C-Dance.png', 0, 0, 0, 1, '2011-11-11', 14014, 0, '2017-01-01 00:00:00', 0), \
('Closure', '', 'http://streamserver.c-dance.com:8320/', '', 'Closure.png', 0, 0, 0, 1, '2011-11-11', 14015, 0, '2017-01-01 00:00:00', 0), \
('Annals-of-Jazz', '', 'http://streamserver.c-dance.com:8320/', '', 'Annals-of-Jazz.png', 0, 0, 0, 1, '2011-11-11', 14016, 0, '2017-01-01 00:00:00', 0), \
('The-Wood-891', '', 'http://www.live365.com/play/kclchd1', 'http://www.crossroadsradio.net/', 'The-Wood-891.png', 0, 0, 0, 1, '2011-11-11', 14017, 0, '2017-01-01 00:00:00', 0), \
('WKXM-1300', '', 'http://www.live365.com/play/kclchd1', '', 'WKXM-1300.png', 0, 0, 0, 1, '2011-11-11', 14018, 0, '2017-01-01 00:00:00', 0), \
('Hall--Oates', '', 'http://www.live365.com/play/kclchd1', '', 'Hall--Oates.png', 0, 0, 0, 1, '2011-11-11', 14019, 0, '2017-01-01 00:00:00', 0), \
('KLBQ-987', '', 'http://www.live365.com/play/kclchd1', 'http://www.987klbq.com', 'KLBQ-987.png', 0, 0, 0, 1, '2011-11-11', 14020, 0, '2017-01-01 00:00:00', 0), \
('The-Orb', '', 'http://www.live365.com/play/kclchd1', '', 'The-Orb.png', 0, 0, 0, 1, '2011-11-11', 14021, 0, '2017-01-01 00:00:00', 0), \
('Martin-Rapp', '', 'http://www.live365.com/play/kclchd1', '', 'Martin-Rapp.png', 0, 0, 0, 1, '2011-11-11', 14022, 0, '2017-01-01 00:00:00', 0), \
('KISN-967', '', 'http://www.abc.net.au/res/streaming/audio/aac/radio_australia_fra.pls', 'http://www.bozemanskissfm.com/', 'KISN-967.png', 0, 0, 0, 1, '2011-11-11', 14024, 0, '2017-01-01 00:00:00', 0), \
('KSJS-905', '', 'http://www.abc.net.au/res/streaming/audio/aac/radio_australia_fra.pls', 'http://www.ksjs.org/', 'KSJS-905.png', 0, 0, 0, 1, '2011-11-11', 14025, 0, '2017-01-01 00:00:00', 0), \
('WFMB-FM-1045', '', 'http://www.abc.net.au/res/streaming/audio/aac/radio_australia_fra.pls', 'http://www.wfmb.com/', 'WFMB-FM-1045.png', 0, 0, 0, 1, '2011-11-11', 14026, 0, '2017-01-01 00:00:00', 0), \
('JENNiRADIO-1240', 'KALY AM 1240 is owned by the youngest radio station owner in the US, 15-year-old Jennifer Smart, through her charitable foundation.  It broadcasts the JENNiRADIO format, aimed at kids and families, wh', 'http://loudcity.com/stations/jenniradio/files/show/pls_mp3.pls', 'http://www.jenniradio.com', 'JENNiRADIO-1240.png', 0, 0, 0, 1, '2011-11-11', 14027, 0, '2017-01-01 00:00:00', 0), \
('Classical-Public-Radio-899', '', 'http://www.wdav.org/streams/WDAV-128k.m3u', 'http://www.wdav.org/', 'Classical-Public-Radio-899.png', 0, 0, 0, 1, '2011-11-11', 14028, 0, '2017-01-01 00:00:00', 0), \
('The-Freddie-Coleman-Show', '', 'http://www.wdav.org/streams/WDAV-128k.m3u', '', 'The-Freddie-Coleman-Show.png', 0, 0, 0, 1, '2011-11-11', 14029, 0, '2017-01-01 00:00:00', 0), \
('Split-Enz', '', 'http://www.wdav.org/streams/WDAV-128k.m3u', '', 'Split-Enz.png', 0, 0, 0, 1, '2011-11-11', 14030, 0, '2017-01-01 00:00:00', 0), \
('The-Tom-OBrien-Show', '', 'http://www.wdav.org/streams/WDAV-128k.m3u', '', 'The-Tom-OBrien-Show.png', 0, 0, 0, 1, '2011-11-11', 14031, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Fun', '', 'http://www.wdav.org/streams/WDAV-128k.m3u', '', 'Sunday-Fun.png', 0, 0, 0, 1, '2011-11-11', 14032, 0, '2017-01-01 00:00:00', 0), \
('Level-42', '', 'http://www.wdav.org/streams/WDAV-128k.m3u', '', 'Level-42.png', 0, 0, 0, 1, '2011-11-11', 14033, 0, '2017-01-01 00:00:00', 0), \
('WJCU-887', '', 'http://wjcu.jcu.edu:8001', 'http://www.wjcu.org/', 'WJCU-887.png', 0, 0, 0, 1, '2011-11-11', 14035, 0, '2017-01-01 00:00:00', 0), \
('Time-Out-with-TA', '', 'http://wjcu.jcu.edu:8001', '', 'Time-Out-with-TA.png', 0, 0, 0, 1, '2011-11-11', 14036, 0, '2017-01-01 00:00:00', 0), \
('Iggy-Pop', '', 'http://wjcu.jcu.edu:8001', '', 'Iggy-Pop.png', 0, 0, 0, 1, '2011-11-11', 14037, 0, '2017-01-01 00:00:00', 0), \
('EZ-Rock-975', '', 'http://wjcu.jcu.edu:8001', 'http://www.975ezrock.com/', 'EZ-Rock-975.png', 0, 0, 0, 1, '2011-11-11', 14038, 0, '2017-01-01 00:00:00', 0), \
('Q-997', 'KMBQ radio, or Q 997, serves the Matanuska/Susitna Valley and beyond. We can be heard from Willow to Girdwood and every place in between.<BR>KMBQ-FM (99.7 FM) is a radio station broadcasting an adult ', 'http://www.live365.com/play/kmbq', 'http://www.kmbq.com/', 'Q-997.png', 0, 0, 0, 1, '2011-11-11', 14039, 0, '2017-01-01 00:00:00', 0), \
('181FM-Super-70s', '', 'http://listen.181fm.com/181-70s_128k.mp3', 'http://www.181.fm/', '181FM-Super-70s.png', 0, 0, 0, 1, '2011-11-11', 14040, 0, '2017-01-01 00:00:00', 1), \
('Global-American-Broadcasting-1', '', 'http://www.gabradionetwork.com/gab1.asx', 'http://www.gabradionetwork.com/index.htm', 'Global-American-Broadcasting-1.png', 0, 0, 0, 1, '2011-11-11', 14041, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1350', '', 'http://www.gabradionetwork.com/gab1.asx', 'http://www.1350espn.com/', 'ESPN-1350.png', 0, 0, 0, 1, '2011-11-11', 14042, 0, '2017-01-01 00:00:00', 0), \
('Halo', '', 'http://www.gabradionetwork.com/gab1.asx', '', 'Halo.png', 0, 0, 0, 1, '2011-11-11', 14043, 0, '2017-01-01 00:00:00', 0), \
('Dinah-Washington', '', 'http://www.gabradionetwork.com/gab1.asx', '', 'Dinah-Washington.png', 0, 0, 1, 1, '2011-11-11', 14044, 0, '2011-10-17 23:43:43', 0), \
('Underground-Progressive-Radio', 'Underground Progressive Radio is an internet and part 15 station located in Brick, NJ.', 'http://s7.myradiostream.com/27564.pls', 'http://undergroundprogressiveradio.webs.com/', 'Underground-Progressive-Radio.png', 0, 0, 0, 1, '2011-11-11', 14045, 0, '2017-01-01 00:00:00', 0), \
('Aberdeen', '', 'http://s7.myradiostream.com/27564.pls', '', 'Aberdeen.png', 0, 0, 0, 1, '2011-11-11', 14046, 0, '2017-01-01 00:00:00', 0), \
('Fishing-Headquarters-Live', '', 'http://s7.myradiostream.com/27564.pls', '', 'Fishing-Headquarters-Live.png', 0, 0, 0, 1, '2011-11-11', 14047, 0, '2017-01-01 00:00:00', 0), \
('WRGA-1470', 'WRGA is your home for NewsTalk including Neal Boortz, Clark Howard, Sean Hannity and local news for Floyd and surrounding counties throughout the day.', 'http://s7.myradiostream.com/27564.pls', 'http://www.wrgarome.com/', 'WRGA-1470.png', 0, 0, 0, 1, '2011-11-11', 14048, 0, '2017-01-01 00:00:00', 0), \
('Inspiration-Hour', '', 'http://s7.myradiostream.com/27564.pls', '', 'Inspiration-Hour.png', 0, 0, 0, 1, '2011-11-11', 14049, 0, '2017-01-01 00:00:00', 0), \
('Prime-Time-Radio', '', 'http://s7.myradiostream.com/27564.pls', '', 'Prime-Time-Radio.png', 0, 0, 0, 1, '2011-11-11', 14050, 0, '2017-01-01 00:00:00', 0), \
('1037-Lite-FM', '103.7 Lite-FM Today&#39;s Refreshing Lite Rock', 'http://www.abc.net.au/res/streaming/audio/aac/radio_australia_mya.pls', 'http://www.1037litefm.com/', '1037-Lite-FM.png', 0, 0, 0, 1, '2011-11-11', 14054, 0, '2017-01-01 00:00:00', 0), \
('Radio-538-Nonstop-40', '', 'http://82.201.100.23/WEB02-538NonStop40.m3u', 'http://www.radio538.nl/', 'Radio-538-Nonstop-40.png', 0, 0, 0, 1, '2011-11-11', 14055, 0, '2017-01-01 00:00:00', 0), \
('Bonnie-Raitt', '', 'http://82.201.100.23/WEB02-538NonStop40.m3u', '', 'Bonnie-Raitt.png', 0, 0, 0, 1, '2011-11-11', 14056, 0, '2017-01-01 00:00:00', 0), \
('The-Sound-1003', '', 'http://82.201.100.23/WEB02-538NonStop40.m3u', 'http://www.thesoundla.com/', 'The-Sound-1003.png', 0, 0, 0, 1, '2011-11-11', 14057, 0, '2017-01-01 00:00:00', 0), \
('Porn-Kings', '', 'http://82.201.100.23/WEB02-538NonStop40.m3u', '', 'Porn-Kings.png', 0, 0, 0, 1, '2011-11-11', 14058, 0, '2017-01-01 00:00:00', 0), \
('Magic-1013', '', 'http://82.201.100.23/WEB02-538NonStop40.m3u', 'http://www.magic1013.com/', 'Magic-1013.png', 0, 0, 0, 1, '2011-11-11', 14059, 0, '2017-01-01 00:00:00', 0), \
('Talking-Football', '', 'http://82.201.100.23/WEB02-538NonStop40.m3u', '', 'Talking-Football.png', 0, 0, 0, 1, '2011-11-11', 14060, 0, '2017-01-01 00:00:00', 0), \
('RadioIO-Acoustic-Cafe', 'A lighter version of adult album alternative music, focusing on the acoustic songs by renowned contemporary singer-songwriters, and some that may be new to you, along with acoustic versions of favorit', 'http://streampoint.radioio.com/streams/92/46f28fc4c4be5/listen.pls', 'http://www.radioio.com/channels/acoustic-cafe', 'RadioIO-Acoustic-Cafe.png', 0, 0, 0, 1, '2011-11-11', 14061, 0, '2017-01-01 00:00:00', 0), \
('Radio-Exit-1064', '', 'http://89.238.146.146:7010/', 'http://www.radioexit.com/', 'Radio-Exit-1064.png', 0, 0, 0, 1, '2011-11-11', 14062, 0, '2017-01-01 00:00:00', 0), \
('Scarborough', '', 'http://89.238.146.146:7010/', '', 'Scarborough-vs-Deering-Oct-6-2011.png', 0, 0, 0, 1, '2011-11-11', 14063, 0, '2017-01-01 00:00:00', 0), \
('DJ-Khaled', '', 'http://89.238.146.146:7010/', '', 'DJ-Khaled.png', 0, 0, 0, 1, '2011-11-11', 14064, 0, '2017-01-01 00:00:00', 0), \
('pure-fm-972', '97.2 pure fm - berlins dance radio spielt den Beat der Hauptstadt - house, electro, dance, pop und disco. 97.2 pure fm spielt den Sound der Metropole', 'http://www.radiostream.de/stream/37472.pls', 'http://www.purefm.de', 'pure-fm-972.png', 0, 0, 0, 1, '2011-11-11', 14065, 0, '2017-01-01 00:00:00', 0), \
('Ukcountryradiocom', '', 'http://ukcountryradio.com/ukcr.m3u', 'http://ukcountryradio.com/', 'Ukcountryradiocom.png', 0, 0, 0, 1, '2011-11-11', 14066, 0, '2017-01-01 00:00:00', 0), \
('Super-Estrella-1071', '', 'http://ukcountryradio.com/ukcr.m3u', 'http://www.superestrella.com/', 'Super-Estrella-1071.png', 0, 0, 0, 1, '2011-11-11', 14067, 0, '2017-01-01 00:00:00', 0), \
('MIX-1051', 'MIX105.1 plays the best music mix from the 80&#39;s, 90&#39;s &amp; today. Visit mix1051.com to subscribe to the MIX Plus e-mail club and to listen live! MIX105.1 supports numerous community events su', 'http://ukcountryradio.com/ukcr.m3u', 'http://www.mix1051.com', 'MIX-1051.png', 0, 0, 0, 1, '2011-11-11', 14069, 0, '2017-01-01 00:00:00', 0), \
('WKVT-FM-927', 'Brattleboro&#39;s, Keene&#39;s and Greenfield&#39;s choice for Classic Rock.', 'http://ukcountryradio.com/ukcr.m3u', 'http://www.wkvt.com/', 'WKVT-FM-927.png', 0, 0, 0, 1, '2011-11-11', 14070, 0, '2017-01-01 00:00:00', 0), \
('SKYFM-Urban-Jamz', 'Kickin&#39; with the baddest beats on the &#39;net', 'http://listen.sky.fm/partner_mp3/urbanjamz.pls', 'http://www.sky.fm/urbanjamz', 'SKYFM-Urban-Jamz.png', 0, 0, 0, 1, '2011-11-11', 14071, 0, '2017-01-01 00:00:00', 0), \
('The-Score-1400', '', 'http://listen.sky.fm/partner_mp3/urbanjamz.pls', '', 'The-Score-1400.png', 0, 0, 0, 1, '2011-11-11', 14072, 0, '2017-01-01 00:00:00', 0), \
('Robert-Cray', '', 'http://listen.sky.fm/partner_mp3/urbanjamz.pls', '', 'Robert-Cray.png', 0, 0, 0, 1, '2011-11-11', 14073, 0, '2017-01-01 00:00:00', 0), \
('The-Bobby-Jones-Gospel-Countdown', '', 'http://listen.sky.fm/partner_mp3/urbanjamz.pls', '', 'The-Bobby-Jones-Gospel-Countdown.png', 0, 0, 0, 1, '2011-11-11', 14074, 0, '2017-01-01 00:00:00', 0), \
('Talk-Radio-1270', '', 'http://pub5.radiotunes.com:80/radiotunes_urbanjamz', 'http://www.wgsv.com/', 'Talk-Radio-1270.png', 0, 0, 0, 1, '2011-11-11', 14075, 0, '2017-01-01 00:00:00', 1), \
('The-Undertones', '', 'http://pub5.radiotunes.com:80/radiotunes_urbanjamz', '', 'The-Undertones.png', 0, 0, 0, 1, '2011-11-11', 14076, 0, '2017-01-01 00:00:00', 0), \
('Slow-Jam', 'Une radio Slow Jam pour des France.', 'http://listen.radionomy.com/slow-jam', 'http://www.radionomy.com/en/radio/slow-jam', 'Slow-Jam.png', 0, 0, 0, 1, '2011-11-11', 14077, 0, '2017-01-01 00:00:00', 1), \
('Iron-Maiden', '', 'http://rclive.ath.cx:8282/mp3', '', 'Iron-Maiden.png', 0, 0, 0, 1, '2011-11-11', 14086, 0, '2017-01-01 00:00:00', 0), \
('WBFG-965', '', 'http://rclive.ath.cx:8282/mp3', '', 'WBFG-965.png', 0, 0, 0, 1, '2011-11-11', 14088, 0, '2017-01-01 00:00:00', 0), \
('AlterNative-Voices', '', 'http://rclive.ath.cx:8282/mp3', '', 'AlterNative-Voices.png', 0, 0, 0, 1, '2011-11-11', 14089, 0, '2017-01-01 00:00:00', 0), \
('Antenne-Bayern-1030', 'Das beste aus den 80ern, 90ern und den Hits von heute.', 'http://www.antenne.de/webradio/antenne.aac.pls', 'http://www.antenne.de/', 'Antenne-Bayern-1030.png', 0, 0, 0, 1, '2011-11-11', 14090, 0, '2017-01-01 00:00:00', 0), \
('WWIS-1260', 'Tune to AM1260 to hear the best music from the 70&#39;s, 80&#39;s, 90&#39;s and today, including some from the 60&#39;s.  There&#39;s CBS News on the hour, most hours.<BR><BR>Be sure to listen Friday ', 'http://ice5.securenetsystems.net:80/WWISAM', 'http://www.wwisradio.com/', 'WWIS-1260.png', 0, 0, 0, 1, '2011-11-11', 14091, 0, '2017-01-01 00:00:00', 0), \
('The-Wallflowers', '', 'http://ice5.securenetsystems.net:80/WWISAM', '', 'The-Wallflowers.png', 0, 0, 0, 1, '2011-11-11', 14092, 0, '2017-01-01 00:00:00', 0), \
('Bossa-Nova-on-JAZZRADIOcom', 'Sweet sounds of bossa nova and the flavors of Brazil.', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', 'http://www.jazzradio.com/', 'Bossa-Nova-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 14093, 0, '2017-01-01 00:00:00', 1), \
('Jeremy-Vine', '', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', '', 'Jeremy-Vine.png', 0, 0, 0, 1, '2011-11-11', 14094, 0, '2017-01-01 00:00:00', 1), \
('Ralph-Tees-Luxury-Soul', '', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', '', 'Ralph-Tees-Luxury-Soul.png', 0, 0, 0, 1, '2011-11-11', 14095, 0, '2017-01-01 00:00:00', 1), \
('Charlie-Winston', '', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', '', 'Charlie-Winston.png', 0, 0, 0, 1, '2011-11-11', 14097, 0, '2017-01-01 00:00:00', 1), \
('Christmas-Melody', '', 'http://www.xmasmelody.com/128.asx', 'http://www.xmasmelody.com/', 'Christmas-Melody.png', 0, 0, 0, 1, '2011-11-11', 14098, 0, '2017-01-01 00:00:00', 0), \
('Big-Country-1009', '', 'http://asx.abacast.com/eldorado-kxfm-64.pls', 'http://www.wwbr.com/', 'Big-Country-1009.png', 0, 0, 0, 1, '2011-11-11', 14100, 0, '2017-01-01 00:00:00', 0), \
('Christophe', '', 'http://static.abradio.cz/data/s/20/playlist/madonna64.asx', '', '', 0, 0, 0, 1, '2011-11-11', 14103, 0, '2017-01-01 00:00:00', 0), \
('Evening-Jazz-(KCSM)', '', 'http://static.abradio.cz/data/s/20/playlist/madonna64.asx', '', 'Evening-Jazz-(KCSM).png', 0, 0, 0, 1, '2011-11-11', 14104, 0, '2017-01-01 00:00:00', 0), \
('WSMN-1590', '', 'http://amber.streamguys.com:4570', 'http://www.wsmnradio.com/', 'WSMN-1590.png', 0, 0, 0, 1, '2011-11-11', 14105, 0, '2017-01-01 00:00:00', 0), \
('WJPF-1340', '', 'http://amber.streamguys.com:4570', 'http://www.wjpf.com/', 'WJPF-1340.png', 0, 0, 0, 1, '2011-11-11', 14106, 0, '2017-01-01 00:00:00', 0), \
('Match-Day-Live-Preview', '', 'http://amber.streamguys.com:4570', '', 'Match-Day-Live-Preview.png', 0, 0, 0, 1, '2011-11-11', 14107, 0, '2017-01-01 00:00:00', 0), \
('99-X-993', '', 'http://amber.streamguys.com:4570', 'http://www.99xwjbx.com/', '99-X-993.png', 0, 0, 1, 1, '2011-11-11', 14108, 0, '2011-10-17 23:40:45', 1), \
('Fox-Sports-730', '', 'http://amber.streamguys.com:4570', 'http://www.sportsradio730.com/', 'Fox-Sports-730.png', 0, 0, 0, 1, '2011-11-11', 14109, 0, '2017-01-01 00:00:00', 0), \
('Nat-King-Cole', '', 'http://amber.streamguys.com:4570', '', 'Nat-King-Cole.png', 0, 0, 0, 1, '2011-11-11', 14110, 0, '2017-01-01 00:00:00', 0), \
('Indie-979', 'We play a ton of music from many genres that were relatively untapped by radio until now. We play everything from Post Punk (Gang of Four, Public Image Limited, Joy Divison), to early alternative (R.E', 'http://amber.streamguys.com:4570', 'http://www.indie979.com/', 'Indie-979.png', 0, 0, 0, 1, '2011-11-11', 14111, 0, '2017-01-01 00:00:00', 0), \
('Rock-102-1021', '', 'http://amber.streamguys.com:4570', 'http://www.rock102.com/', 'Rock-102-1021.png', 0, 0, 0, 1, '2011-11-11', 14112, 0, '2017-01-01 00:00:00', 0), \
('KFBK-1530', '', 'http://amber.streamguys.com:4570', 'http://www.kfbk.com/', 'KFBK-1530.png', 0, 0, 0, 1, '2011-11-11', 14113, 0, '2017-01-01 00:00:00', 0), \
('WRBC-Smile-Radio', '', 'http://ss3.amfmph.com:82/smileradio', 'http://www.wrbcsmileradio.tk/', 'WRBC-Smile-Radio.png', 0, 0, 0, 1, '2011-11-11', 14114, 0, '2017-01-01 00:00:00', 0), \
('Fine-Young-Cannibals', '', 'http://94.23.216.118:8022/', '', 'Fine-Young-Cannibals.png', 0, 0, 0, 1, '2011-11-11', 14120, 0, '2017-01-01 00:00:00', 0), \
('Faithless', '', 'http://94.23.216.118:8022/', '', 'Faithless.png', 0, 0, 0, 1, '2011-11-11', 14121, 0, '2017-01-01 00:00:00', 0), \
('KGB-FM-1015', '', 'http://94.23.216.118:8022/', 'http://www.101kgb.com/', 'KGB-FM-1015.png', 0, 0, 0, 1, '2011-11-11', 14122, 0, '2017-01-01 00:00:00', 0), \
('ESPN-Radio-1400', '', 'http://94.23.216.118:8022/', 'http://www.espn1400.com/', 'ESPN-Radio-1400.png', 0, 0, 0, 1, '2011-11-11', 14123, 0, '2017-01-01 00:00:00', 0), \
('The-Groove-931', '', 'http://sc8.spacialnet.com:25826', 'http://www.thegroove.org/', 'The-Groove-931.png', 0, 0, 0, 1, '2011-11-11', 14124, 0, '2017-01-01 00:00:00', 0), \
('Oldies-1033', '', 'http://sc8.spacialnet.com:25826', 'http://www.klou.com/', 'Oldies-1033.png', 0, 0, 0, 1, '2011-11-11', 14125, 0, '2017-01-01 00:00:00', 0), \
('BlackBeatsFM', '24/7 best hip hop , r &amp; b, house &amp; electro beats, crunk, reggaeton, urban, partybreaks, black music, remix. Live in an amazing mix the cream of the crop.', 'http://www.blackbeats.fm/listen.m3u', 'http://www.blackbeats.fm/', 'BlackBeatsFM.png', 0, 0, 0, 1, '2011-11-11', 14126, 0, '2017-01-01 00:00:00', 0), \
('BAYERN-3-973', 'Bayern 3', 'http://streams.br-online.de/bayern3_2.m3u', 'http://www.br-online.de/bayern3/', 'BAYERN-3-973.png', 0, 0, 0, 1, '2011-11-11', 14127, 0, '2017-01-01 00:00:00', 0), \
('Tee-to-Green', '', 'http://streams.br-online.de/bayern3_2.m3u', '', 'Tee-to-Green.png', 0, 0, 0, 1, '2011-11-11', 14128, 0, '2017-01-01 00:00:00', 0), \
('Ashdown-FM-90s-Hits', '', 'http://us5.my-shoutcast.net:9090', 'http://www.ashdownfm.com/', 'Ashdown-FM-90s-Hits.png', 0, 0, 0, 1, '2011-11-11', 14129, 0, '2017-01-01 00:00:00', 0), \
('Squeeze', '', 'http://us5.my-shoutcast.net:9090', '', 'Squeeze.png', 0, 0, 0, 1, '2011-11-11', 14130, 0, '2017-01-01 00:00:00', 0), \
('The-Best-of-Rush-Limbaugh', '', 'http://us5.my-shoutcast.net:9090', '', 'The-Best-of-Rush-Limbaugh.png', 0, 0, 0, 1, '2011-11-11', 14131, 0, '2017-01-01 00:00:00', 0), \
('Pierpoljak', '', 'http://us5.my-shoutcast.net:9090', '', 'Pierpoljak.png', 0, 0, 0, 1, '2011-11-11', 14132, 0, '2017-01-01 00:00:00', 0), \
('Howie-Day', '', 'http://us5.my-shoutcast.net:9090', '', 'Howie-Day.png', 0, 0, 0, 1, '2011-11-11', 14133, 0, '2017-01-01 00:00:00', 0), \
('Radio-Amor-1075', '', 'http://us5.my-shoutcast.net:9090', 'http://www.amor1075fm.com', 'Radio-Amor-1075.png', 0, 0, 0, 1, '2011-11-11', 14134, 0, '2017-01-01 00:00:00', 0), \
('The-Classic-1230', 'The day was the 18th of May when Orvall Burda and his original radio stars went on the air here at KDIX in 1947. In those days, they were indeed stars to the local public. Orvall Burda was the first M', 'http://www.live365.com/cgi-bin/wmp.asx?name_type=nam&station=kdixam', 'http://www.kdix.net/', 'The-Classic-1230.png', 0, 0, 0, 1, '2011-11-11', 14135, 0, '2017-01-01 00:00:00', 0), \
('BBC-Radio-Wales-1039', '', 'http://bbc.co.uk/radio/listen/live/rw.asx', 'http://www.bbc.co.uk/wales/radiowales/', 'BBC-Radio-Wales-1039.png', 0, 0, 0, 1, '2011-11-11', 14138, 0, '2017-01-01 00:00:00', 0), \
('WFNR-710', '', 'mms://live.cumulusstreaming.com/WFNR-AM', 'http://www.710wfnr.com/', 'WFNR-710.png', 0, 0, 0, 1, '2011-11-11', 14139, 0, '2017-01-01 00:00:00', 0), \
('Life-1071', '', 'http://nwmedia-knwi.streamguys.com/knwi', 'http://www.desmoines.fm/', 'Life-1071.png', 0, 0, 0, 1, '2011-11-11', 14140, 0, '2017-01-01 00:00:00', 0), \
('KDHX-881', '88.1 KDHX broadcasts diverse music programming from St. Louis and worldwide via the web 24 hours a day, 7 days a week. KDHX supports music, the arts and culture in St. Louis through media arts outreac', 'http://stream.kdhx.org:8000/listen.pls', 'http://www.kdhx.org/', 'KDHX-881.png', 0, 0, 0, 1, '2011-11-11', 14141, 0, '2017-01-01 00:00:00', 0), \
('WSBR-740', '', 'mms://streamer3.securenetsystems.net/WSBR', 'http://www.wsbrradio.com/', 'WSBR-740.png', 0, 0, 0, 1, '2011-11-11', 14142, 0, '2017-01-01 00:00:00', 0), \
('Joe-Goddard', '', 'mms://streamer3.securenetsystems.net/WSBR', '', 'Joe-Goddard.png', 0, 0, 0, 1, '2011-11-11', 14143, 0, '2017-01-01 00:00:00', 0), \
('KMMJ-750', '', 'http://69.10.0.2/kmmj', 'http://kmmj.org/', 'KMMJ-750.png', 0, 0, 0, 1, '2011-11-11', 14144, 0, '2017-01-01 00:00:00', 0), \
('Jaki-Graham', '', 'http://69.10.0.2/kmmj', '', 'Jaki-Graham.png', 0, 0, 0, 1, '2011-11-11', 14145, 0, '2017-01-01 00:00:00', 0), \
('DR P4 jylland', '', 'http://live-icy.gss.dr.dk:8000/A/A10H.mp3', 'http://www.dr.dk/p4/aarhus/', 'DR-P4-jylland-959.png', 0, 0, 5, 1, '2011-11-11', 14150, 0, '2011-10-14 16:08:24', 1), \
('KTKS-951', '', 'mms://stream1.securenetsystems.net/KTKS', 'http://www.lakeradio.com/', 'KTKS-951.png', 0, 0, 0, 1, '2011-11-11', 14151, 0, '2017-01-01 00:00:00', 0), \
('WCQM-983', '', 'http://in.icy2.abacast.com/heartlandcom-wcqmfm-32.m3u', 'http://www.wcqm.com/', 'WCQM-983.png', 0, 0, 0, 1, '2011-11-11', 14152, 0, '2017-01-01 00:00:00', 0), \
('La-Raza-Rocks', '', 'http://in.icy2.abacast.com/heartlandcom-wcqmfm-32.m3u', '', 'La-Raza-Rocks.png', 0, 0, 0, 1, '2011-11-11', 14153, 0, '2017-01-01 00:00:00', 0), \
('The-Mighty-790', '', 'http://icy2.abacast.com/radiofm-kfgoaac-64.m3u', 'http://www.kfgo.com/', 'The-Mighty-790.png', 0, 0, 0, 1, '2011-11-11', 14154, 0, '2017-01-01 00:00:00', 0), \
('Sundays-with-Sanden', '', 'http://icy2.abacast.com/radiofm-kfgoaac-64.m3u', '', 'Sundays-with-Sanden.png', 0, 0, 0, 1, '2011-11-11', 14155, 0, '2017-01-01 00:00:00', 0), \
('KWYN-1400', '', 'http://icy2.abacast.com/radiofm-kfgoaac-64.m3u', 'http://www.kwyn.com', 'KWYN-1400.png', 0, 0, 0, 1, '2011-11-11', 14156, 0, '2017-01-01 00:00:00', 0), \
('Robbers-on-High-Street', '', 'http://icy2.abacast.com/radiofm-kfgoaac-64.m3u', '', 'Robbers-on-High-Street.png', 0, 0, 0, 1, '2011-11-11', 14157, 0, '2017-01-01 00:00:00', 0), \
('Jazz-Set', '', 'http://icy2.abacast.com/radiofm-kfgoaac-64.m3u', '', 'Jazz-Set.png', 0, 0, 0, 1, '2011-11-11', 14158, 0, '2017-01-01 00:00:00', 0), \
('Alabama', '', 'http://icy2.abacast.com/radiofm-kfgoaac-64.m3u', '', 'Alabama.png', 0, 0, 0, 1, '2011-11-11', 14159, 0, '2017-01-01 00:00:00', 0), \
('ROCK-ANTENNE-Heavy-Metal', '', 'http://www.rockantenne.de/webradio/channels/heavy-metal.aac.pls', 'http://www.rockantenne.de/', 'ROCK-ANTENNE-Heavy-Metal.png', 0, 0, 0, 1, '2011-11-11', 14160, 0, '2017-01-01 00:00:00', 0), \
('KHAT-1210', '', 'http://www.rockantenne.de/webradio/channels/heavy-metal.aac.pls', '', 'KHAT-1210.png', 0, 0, 0, 1, '2011-11-11', 14161, 0, '2017-01-01 00:00:00', 0), \
('WRHIcom-Football-Stream-4', '', 'http://www.rockantenne.de/webradio/channels/heavy-metal.aac.pls', 'http://wrhi.com', 'WRHIcom-Football-Stream-4.png', 0, 0, 0, 1, '2011-11-11', 14162, 0, '2017-01-01 00:00:00', 0), \
('Toto-Coelo', '', 'http://www.rockantenne.de/webradio/channels/heavy-metal.aac.pls', '', 'Toto-Coelo.png', 0, 0, 0, 1, '2011-11-11', 14163, 0, '2017-01-01 00:00:00', 0), \
('Oxford-Hill-vs-Edward-Little-Oct-21-2011', '', 'http://www.rockantenne.de/webradio/channels/heavy-metal.aac.pls', '', 'Oxford-Hill-vs-Edward-Little-Oct-21-2011.png', 0, 0, 0, 1, '2011-11-11', 14164, 0, '2017-01-01 00:00:00', 0), \
('WIST-690', 'Locally owned and operated, 690 WIST Radio brings New Orleans it&#39;s first locally produced sports talk show daily at 6am with the Morning Drive with Brian Allee Walsh. Local talk continues till 6pm', 'http://wmc1.den.liquidcompass.net/WISTAM', 'http://wistradio.com/', 'WIST-690.png', 0, 0, 0, 1, '2011-11-11', 14165, 0, '2017-01-01 00:00:00', 0), \
('Album-88-885', '', 'http://wrasnetstreamer.gsu.edu:1755', 'http://www.wras.org', 'Album-88-885.png', 0, 0, 0, 1, '2011-11-11', 14166, 0, '2017-01-01 00:00:00', 0), \
('WUPS-985', '', 'http://www3.streamgeeks.com:7010', 'http://www.wups.com/', 'WUPS-985.png', 0, 0, 0, 1, '2011-11-11', 14167, 0, '2017-01-01 00:00:00', 0), \
('Pink-Radio-International', '', 'http://89.149.245.235/', 'http://pinkradio.com/', 'Pink-Radio-International.png', 0, 0, 0, 1, '2011-11-11', 14168, 0, '2017-01-01 00:00:00', 0), \
('WRGM-1440', '', 'http://89.149.245.235/', '', 'WRGM-1440.png', 0, 0, 0, 1, '2011-11-11', 14169, 0, '2017-01-01 00:00:00', 0), \
('SportsCenter-Saturday', '', 'http://89.149.245.235/', '', 'SportsCenter-Saturday.png', 0, 0, 0, 1, '2011-11-11', 14170, 0, '2017-01-01 00:00:00', 0), \
('88Nine-Radio-889', '', 'http://216.246.14.71/', 'http://www.radiomilwaukee.org/', '88Nine-Radio-889.png', 0, 0, 0, 1, '2011-11-11', 14171, 0, '2017-01-01 00:00:00', 0), \
('Power-Gospel-with-JC', '', 'http://216.246.14.71/', '', 'Power-Gospel-with-JC.png', 0, 0, 0, 1, '2011-11-11', 14172, 0, '2017-01-01 00:00:00', 0), \
('Junior-Boys', '', 'http://216.246.14.71/', '', 'Junior-Boys.png', 0, 0, 0, 1, '2011-11-11', 14173, 0, '2017-01-01 00:00:00', 0), \
('KYUL-1310', '', 'http://216.246.14.71/', 'http://www.kiulradio.com/', 'KYUL-1310.png', 0, 0, 0, 1, '2011-11-11', 14174, 0, '2017-01-01 00:00:00', 0), \
('Oingo-Boingo', '', 'http://216.246.14.71/', '', 'Oingo-Boingo.png', 0, 0, 0, 1, '2011-11-11', 14175, 0, '2017-01-01 00:00:00', 0), \
('WTKM-FM-1049', '', 'http://www.warpradio.com/player/mediaserver.asp?id=11069&t=2&streamRate=', 'http://www.wtkm.com/', 'WTKM-FM-1049.png', 0, 0, 0, 1, '2011-11-11', 14176, 0, '2017-01-01 00:00:00', 0), \
('Central-Michigan-Chippewas', '', 'http://www.warpradio.com/player/mediaserver.asp?id=11069&t=2&streamRate=', '', '', 0, 0, 0, 1, '2011-11-11', 14177, 0, '2017-01-01 00:00:00', 0), \
('HIM', '', 'http://www.warpradio.com/player/mediaserver.asp?id=11069&t=2&streamRate=', '', 'HIM.png', 0, 0, 0, 1, '2011-11-11', 14178, 0, '2017-01-01 00:00:00', 0), \
('Art-radio', 'WPS1 Art Radio allows listeners with broadband to hear music (DJ sets, experimental, world, classics), talk (on art, music, books, film), poetry (readings and interviews), lectures, special performanc', 'http://www.live365.com/play/artonair', 'http://www.wps1.org/', 'Art-radio.png', 0, 0, 2, 1, '2011-11-11', 14179, 0, '2011-10-17 23:42:25', 0), \
('The-Beatles-Years', '', 'http://www.live365.com/play/artonair', '', 'The-Beatles-Years.png', 0, 0, 0, 1, '2011-11-11', 14180, 0, '2017-01-01 00:00:00', 0), \
('Chromeo', '', 'http://www.live365.com/play/artonair', '', 'Chromeo.png', 0, 0, 0, 1, '2011-11-11', 14181, 0, '2017-01-01 00:00:00', 0), \
('Mix-1041', '', 'http://www.live365.com/play/artonair', 'http://www.mymix1041.com/', 'Mix-1041.png', 0, 0, 0, 1, '2011-11-11', 14182, 0, '2017-01-01 00:00:00', 0), \
('Justice', '', 'http://www.live365.com/play/artonair', '', 'Justice.png', 0, 0, 0, 1, '2011-11-11', 14183, 0, '2017-01-01 00:00:00', 0), \
('Sunday-Morning-Classics', '', 'http://www.live365.com/play/artonair', '', 'Sunday-Morning-Classics.png', 0, 0, 0, 1, '2011-11-11', 14184, 0, '2017-01-01 00:00:00', 0), \
('Z883-FM', '', 'http://mm11.zradio.org:8002', 'http://www.zradio.org/', 'Z883-FM.png', 0, 0, 0, 1, '2011-11-11', 14185, 0, '2017-01-01 00:00:00', 0), \
('St-Vincent', '', 'http://mm11.zradio.org:8002', '', 'St-Vincent.png', 0, 0, 0, 1, '2011-11-11', 14186, 0, '2017-01-01 00:00:00', 0), \
('Lounge-Music-Radio', '', 'http://89.111.189.3:6495/', 'http://www.loungemusic.ru/', 'Lounge-Music-Radio.png', 0, 0, 0, 1, '2011-11-11', 14187, 0, '2017-01-01 00:00:00', 0), \
('Time-to-Live', '', 'http://89.111.189.3:6495/', '', 'Time-to-Live.png', 0, 0, 0, 1, '2011-11-11', 14188, 0, '2017-01-01 00:00:00', 0), \
('Radio-7-Ulm-1018', 'Radio 7 spielt die beste Musik im S&#252;den. Au&#223;erdem informieren wir Sie auch &#252;ber alles was vor Ihrer Haust&#252;re und im Rest der Welt passiert.', 'http://stream.hoerradar.de/mp3-radio7-256.m3u', 'http://www.radio7.de/', 'Radio-7-Ulm-1018.png', 0, 0, 0, 1, '2011-11-11', 14189, 0, '2017-01-01 00:00:00', 0), \
('Animal-Collective', '', 'http://stream.hoerradar.de/mp3-radio7-256.m3u', '', 'Animal-Collective.png', 0, 0, 0, 1, '2011-11-11', 14190, 0, '2017-01-01 00:00:00', 0), \
('Wes-Montgomery', '', 'http://stream.hoerradar.de/mp3-radio7-256.m3u', '', 'Wes-Montgomery.png', 0, 0, 0, 1, '2011-11-11', 14191, 0, '2017-01-01 00:00:00', 0), \
('GD-Up-Radio', '', 'http://174.36.206.217:8577/', 'http://www.gdupradio.com/', 'GD-Up-Radio.png', 0, 0, 0, 1, '2011-11-11', 14192, 0, '2017-01-01 00:00:00', 0), \
('Fickle-933', '', 'http://sc1.abacast.com:9080/listen.pls', 'http://www.fickle933.com/', 'Fickle-933.png', 0, 0, 0, 1, '2011-11-11', 14193, 0, '2017-01-01 00:00:00', 0), \
('Westwood', '', 'http://sc1.abacast.com:9080/listen.pls', '', 'Westwood.png', 0, 0, 0, 1, '2011-11-11', 14194, 0, '2017-01-01 00:00:00', 0), \
('Pinetop-Perkins', '', 'http://sc1.abacast.com:9080/listen.pls', '', 'Pinetop-Perkins.png', 0, 0, 0, 1, '2011-11-11', 14195, 0, '2017-01-01 00:00:00', 0), \
('Boston-College-Eagles-at-Virginia-Tech-Hokies-Oct-22-2011', '', 'http://sc1.abacast.com:9080/listen.pls', '', '', 0, 0, 0, 1, '2011-11-11', 14196, 0, '2017-01-01 00:00:00', 0), \
('Talk-1200', '', 'http://sc1.abacast.com:9080/listen.pls', 'http://www.talk1200.com/', 'Talk-1200.png', 0, 0, 0, 1, '2011-11-11', 14197, 0, '2017-01-01 00:00:00', 0), \
('DeeGay-Classic', 'The songs who gay people love', 'http://deegay.it/classic/deegayclassic.m3u', 'http://deegay.it/', 'DeeGay-Classic.png', 0, 0, 0, 1, '2011-11-11', 14198, 0, '2017-01-01 00:00:00', 0), \
('Perry-Como', '', 'http://deegay.it/classic/deegayclassic.m3u', '', 'Perry-Como.png', 0, 0, 0, 1, '2011-11-11', 14199, 0, '2017-01-01 00:00:00', 0), \
('KVKI-FM-965', '', 'http://deegay.it/classic/deegayclassic.m3u', 'http://www.kvkionline.com/', 'KVKI-FM-965.png', 0, 0, 0, 1, '2011-11-11', 14200, 0, '2017-01-01 00:00:00', 0), \
('FunX', 'FunX draait de muziekmix van de straat en bespreekt de onderwerpen die jongeren bezighouden.', 'http://www.funx.nl/live/funx-hoog.m3u', 'http://www.funx.nl/', 'FunX.png', 0, 0, 0, 1, '2011-11-11', 14201, 0, '2017-01-01 00:00:00', 0), \
('Sampaikan-Sinar-Ahad', '', 'http://www.funx.nl/live/funx-hoog.m3u', '', 'Sampaikan-Sinar-Ahad.png', 0, 0, 0, 1, '2011-11-11', 14202, 0, '2017-01-01 00:00:00', 0), \
('WBOW-FM-1027', '', 'http://ice5.securenetsystems.net:80/WBOWFM', 'http://www.wbowfm.com/', 'WBOW-FM-1027.png', 0, 0, 0, 1, '2011-11-11', 14203, 0, '2017-01-01 00:00:00', 0), \
('The-Mighty-Wah', '', 'http://ice5.securenetsystems.net:80/WBOWFM', '', 'The-Mighty-Wah.png', 0, 0, 0, 1, '2011-11-11', 14204, 0, '2017-01-01 00:00:00', 0), \
('Pat-Metheny', '', 'http://ice5.securenetsystems.net:80/WBOWFM', '', 'Pat-Metheny.png', 0, 0, 0, 1, '2011-11-11', 14205, 0, '2017-01-01 00:00:00', 0), \
('KTIQ-1660', '', 'http://ice5.securenetsystems.net:80/WBOWFM', '', 'KTIQ-1660.png', 0, 0, 0, 1, '2011-11-11', 14206, 0, '2017-01-01 00:00:00', 0), \
('Les-Doigts-de-lHomme', '', 'http://ice5.securenetsystems.net:80/WBOWFM', '', 'Les-Doigts-de-lHomme.png', 0, 0, 0, 1, '2011-11-11', 14207, 0, '2017-01-01 00:00:00', 0), \
('Simple-Plan', '', 'http://ice5.securenetsystems.net:80/WBOWFM', '', 'Simple-Plan.png', 0, 0, 0, 1, '2011-11-11', 14208, 0, '2017-01-01 00:00:00', 0), \
('Piratenradioch', '', 'http://www.stream24.com/tune-in/r6454.asx', 'http://www.piratenradio.ch/', 'Piratenradioch.png', 0, 0, 0, 1, '2011-11-11', 14209, 0, '2017-01-01 00:00:00', 0), \
('STAR-1023', 'Elvis Duran in the Morning and 10 in a Row all day.', 'mms://live.cumulusstreaming.com/KEHK-FM', 'http://www.starfm1023.com/', 'STAR-1023.png', 0, 0, 0, 1, '2011-11-11', 14210, 0, '2017-01-01 00:00:00', 0), \
('Triple-M-1049', '', 'http://sc.syd.triplem.com.au/listen.pls', 'http://www.triplem.com.au/sydney', 'Triple-M-1049.png', 0, 0, 0, 1, '2011-11-11', 14211, 0, '2017-01-01 00:00:00', 0), \
('Fruit-Bats', '', 'http://sc.syd.triplem.com.au/listen.pls', '', 'Fruit-Bats.png', 0, 0, 0, 1, '2011-11-11', 14212, 0, '2017-01-01 00:00:00', 0), \
('Enigma', '', 'http://www.wcsb.org:8001', '', 'Enigma.png', 0, 0, 0, 1, '2011-11-11', 14216, 0, '2017-01-01 00:00:00', 0), \
('Absolute-Classic-Rock', 'Absolute Classic Rock is the home of the Great British Guarantee.<BR><BR>You&#39;ll hear classics from Led Zeppelin, The Who, The Rolling Stones, Queen and more of Britain&#39;s finest, plus the best ', 'http://network.absoluteradio.co.uk/core/audio/mp3/live.pls?service=vchq', 'http://absoluteclassicrock.co.uk', 'Absolute-Classic-Rock.png', 0, 0, 0, 1, '2011-11-11', 14217, 0, '2017-01-01 00:00:00', 0), \
('Addict-Rock', '', 'http://stream1.addictradio.net:80/addictrock.mp3', 'http://www.addictradio.net/rock/', 'Addict-Rock.png', 0, 0, 0, 1, '2011-11-11', 14218, 0, '2017-01-01 00:00:00', 0), \
('Rosanne-Cash', '', 'http://stream1.addictradio.net:80/addictrock.mp3', '', 'Rosanne-Cash.png', 0, 0, 0, 1, '2011-11-11', 14219, 0, '2017-01-01 00:00:00', 0), \
('The-Kim-Komando-Show', '', 'http://stream1.addictradio.net:80/addictrock.mp3', '', 'The-Kim-Komando-Show.png', 0, 0, 0, 1, '2011-11-11', 14220, 0, '2017-01-01 00:00:00', 0), \
('The-Wiz-1011', '', 'http://stream1.addictradio.net:80/addictrock.mp3', 'http://wiznation.com/', 'The-Wiz-1011.png', 0, 0, 0, 1, '2011-11-11', 14221, 0, '2017-01-01 00:00:00', 0), \
('HIT-MUSIC-ONLY', '', 'http://stream1.addictradio.net:80/addictrock.mp3', '', 'HIT-MUSIC-ONLY.png', 0, 0, 0, 1, '2011-11-11', 14222, 0, '2017-01-01 00:00:00', 0), \
('KSEY-1230', '', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', 'http://radioksey.com/', 'KSEY-1230.png', 0, 0, 0, 1, '2011-11-11', 14223, 0, '2017-01-01 00:00:00', 0), \
('Swayzak', '', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', '', 'Swayzak.png', 0, 0, 0, 1, '2011-11-11', 14224, 0, '2017-01-01 00:00:00', 0), \
('New-Jersey-1015', 'New Jersey 101.5 FM Radio is Proud to Be New Jersey!!!!! News/Talk radio with Jersey&#39;s Favorite Hits all weekend', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', 'http://www.nj1015.com/', 'New-Jersey-1015.png', 0, 0, 0, 1, '2011-11-11', 14225, 0, '2017-01-01 00:00:00', 0), \
('Heart', '', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', '', 'Heart.png', 0, 0, 0, 1, '2011-11-11', 14226, 0, '2017-01-01 00:00:00', 0), \
('KFMB-760', '', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', 'http://www.760kfmb.com/', 'KFMB-760.png', 0, 0, 0, 1, '2011-11-11', 14227, 0, '2017-01-01 00:00:00', 0), \
('The-BUZZ-1300', '', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', 'http://www.1300thebuzz.com/', 'The-BUZZ-1300.png', 0, 0, 0, 1, '2011-11-11', 14228, 0, '2017-01-01 00:00:00', 0), \
('The-Chameleons', '', 'http://stream101.com/mcp/tunein.php/ksey/playlist.pls', '', 'The-Chameleons.png', 0, 0, 0, 1, '2011-11-11', 14229, 0, '2017-01-01 00:00:00', 0), \
('1FM Disco-Ball', '', 'http://sc8.1.fm:7020', 'http://www.1.fm/', '1FM---Disco-Ball.png', 0, 0, 0, 1, '2011-11-11', 14231, 0, '2017-01-01 00:00:00', 0), \
('Lite-Rock-105-1051', '', 'http://sc8.1.fm:7020', 'http://www.lite105.com/', 'Lite-Rock-105-1051.png', 0, 0, 0, 1, '2011-11-11', 14232, 0, '2017-01-01 00:00:00', 0), \
('URN-1350', 'The University of Nottinghams multi award-winning student radio station. We bring you music.', 'http://sc8.1.fm:7020', 'http://www.urn1350.net/', 'URN-1350.png', 0, 0, 0, 1, '2011-11-11', 14233, 0, '2017-01-01 00:00:00', 0), \
('WMXI-981', '', 'http://live.str3am.com:2350/live', 'http://www.wmxi.net/', 'WMXI-981.png', 0, 0, 0, 1, '2011-11-11', 14234, 0, '2017-01-01 00:00:00', 0), \
('ESPN-1530', '', 'http://live.str3am.com:2350/live', 'http://www.espn1530.com/', 'ESPN-1530.png', 0, 0, 0, 1, '2011-11-11', 14235, 0, '2017-01-01 00:00:00', 0), \
('Delilah-After-Dark', '', 'http://live.str3am.com:2350/live', '', 'Delilah-After-Dark.png', 0, 0, 0, 1, '2011-11-11', 14236, 0, '2017-01-01 00:00:00', 0), \
('Husker-Audio', '', 'http://live.str3am.com:2350/live', '', 'Husker-Audio.png', 0, 0, 0, 1, '2011-11-11', 14237, 0, '2017-01-01 00:00:00', 0), \
('Greenfield-Church-of-Christ', '', 'http://live.str3am.com:2350/live', '', 'Greenfield-Church-of-Christ.png', 0, 0, 0, 1, '2011-11-11', 14238, 0, '2017-01-01 00:00:00', 0), \
('Redskins-on-ESPN-980', '', 'http://live.str3am.com:2350/live', '', 'Redskins-on-ESPN-980.png', 0, 0, 0, 1, '2011-11-11', 14239, 0, '2017-01-01 00:00:00', 0), \
('KFRU-1400', '', 'mms://live.cumulusstreaming.com/KFRU-AM', 'http://www.kfru.com/', 'KFRU-1400.png', 0, 0, 0, 1, '2011-11-11', 14240, 0, '2017-01-01 00:00:00', 0), \
('EldoRadio-Alternative', '', 'http://eldowebradio1.newmedia.lu/', 'http://www.eldoradio.lu/', 'EldoRadio-Alternative.png', 0, 0, 0, 1, '2011-11-11', 14241, 0, '2017-01-01 00:00:00', 0), \
('Stylz-FM-961', 'Native Broadcasting Network limited is strategically located in the densely populated community of Port Antonio in Jamaica. The offices are housed at 4 Boundbrook Avenue, Port Antonio P.O., Portland, ', 'http://38.96.148.27:6756/', 'http://www.stylzfm.com', 'Stylz-FM-961.png', 0, 0, 0, 1, '2011-11-11', 14242, 0, '2017-01-01 00:00:00', 0), \
('WKPT-1400', 'The WKPT-AM Radio Network is an Oldies based radio station serving East Tennessee and Southwest Virginia on AM 1400, 1490, and 1590. All Oldies, All the Time.', 'http://38.96.148.27:6756/', 'http://www.wkptam.com/', 'WKPT-1400.png', 0, 0, 0, 1, '2011-11-11', 14243, 0, '2017-01-01 00:00:00', 0), \
('MOViN-1003', '', 'http://ice5.securenetsystems.net:80/WOLF', 'http://www.movin100.com/', 'MOViN-1003.png', 0, 0, 0, 1, '2011-11-11', 14244, 0, '2017-01-01 00:00:00', 0), \
('KTIL-1590', '', 'http://72.10.14.138:8080/KTILAM', 'http://www.ktil-radio.com/', 'KTIL-1590.png', 0, 0, 0, 1, '2011-11-11', 14245, 0, '2017-01-01 00:00:00', 0), \
('Foghat', '', 'http://72.10.14.138:8080/KTILAM', '', 'Foghat.png', 0, 0, 0, 1, '2011-11-11', 14246, 0, '2017-01-01 00:00:00', 0), \
('WCJO-977', '', 'http://72.10.14.138:8080/KTILAM', '', 'WCJO-977.png', 0, 0, 0, 1, '2011-11-11', 14247, 0, '2017-01-01 00:00:00', 0), \
('Metal-Inquisition', '', 'http://72.10.14.138:8080/KTILAM', '', 'Metal-Inquisition.png', 0, 0, 0, 1, '2011-11-11', 14248, 0, '2017-01-01 00:00:00', 0), \
('WFAE-907', '', 'http://wfae-ice.streamguys.net/wfae1.m3u', 'http://www.wfae.org/', 'WFAE-907.png', 0, 0, 0, 1, '2011-11-11', 14249, 0, '2017-01-01 00:00:00', 0), \
('KMIN-980', 'KMIN is live and local with a wide selection of Country music from yesterday and today. KMIN broadcasts live coverage of the Grants High School Pirates Football and Basketball.', 'http://wfae-ice.streamguys.net/wfae1.m3u', 'http://www.kmin980.com/', 'KMIN-980.png', 0, 0, 0, 1, '2011-11-11', 14250, 0, '2017-01-01 00:00:00', 0), \
('WZBX-1065', '', 'http://wfae-ice.streamguys.net/wfae1.m3u', 'http://www.radiostatesboro.com/', 'WZBX-1065.png', 0, 0, 0, 1, '2011-11-11', 14251, 0, '2017-01-01 00:00:00', 0), \
('Real-Country-975', '', 'http://ice3.securenetsystems.net:80/KFTXM', 'http://www.kftx.com/', 'Real-Country-975.png', 0, 0, 0, 1, '2011-11-11', 14252, 0, '2017-01-01 00:00:00', 0), \
('CFM-964', '', 'http://ice3.securenetsystems.net:80/KFTXM', 'http://www.cfmradio.com/', 'CFM-964.png', 0, 0, 0, 1, '2011-11-11', 14253, 0, '2017-01-01 00:00:00', 0), \
('Madeleine-Peyroux', '', 'http://ice3.securenetsystems.net:80/KFTXM', '', 'Madeleine-Peyroux.png', 0, 0, 0, 1, '2011-11-11', 14254, 0, '2017-01-01 00:00:00', 0), \
('Are-We-Alone', '', 'http://ice3.securenetsystems.net:80/KFTXM', '', 'Are-We-Alone.png', 0, 0, 0, 1, '2011-11-11', 14255, 0, '2017-01-01 00:00:00', 0), \
('KHJK-1037', '', 'mms://live.cumulusstreaming.com/KHJK-FM', 'http://www.1037online.com/', 'KHJK-1037.png', 0, 0, 0, 1, '2011-11-11', 14256, 0, '2017-01-01 00:00:00', 0), \
('Looking-Glass', '', 'mms://live.cumulusstreaming.com/KHJK-FM', '', 'Looking-Glass.png', 0, 0, 0, 1, '2011-11-11', 14257, 0, '2017-01-01 00:00:00', 0), \
('WBQK-1079', '', 'mms://live.cumulusstreaming.com/KHJK-FM', 'http://www.wbach.net/', 'WBQK-1079.png', 0, 0, 0, 1, '2011-11-11', 14258, 0, '2017-01-01 00:00:00', 0), \
('Radio-Tunisie24 Dance', 'Among the stations Radio Tunisie24, users can discover Urban Radio Tunisie devoted to the Underground Music Rap Hip-Hop Rb.', 'http://tunisie24.net/radio-tunisie24-dance.m3u', 'http://www.tunisie24.net/', 'Radio-Tunisie24---Dance.png', 0, 0, 0, 1, '2011-11-11', 14259, 0, '2017-01-01 00:00:00', 0), \
('Number-One-FM-1015', '', 'http://1numara.radyoyayini.com:3330/', 'http://www.numberone.com.tr/', 'Number-One-FM-1015.png', 0, 0, 0, 1, '2011-11-11', 14260, 0, '2017-01-01 00:00:00', 0), \
('WWTH-1007', '', 'http://1numara.radyoyayini.com:3330/', '', 'WWTH-1007.png', 0, 0, 0, 1, '2011-11-11', 14261, 0, '2017-01-01 00:00:00', 0), \
('Haddaway', '', 'http://1numara.radyoyayini.com:3330/', '', 'Haddaway.png', 0, 0, 0, 1, '2011-11-11', 14262, 0, '2017-01-01 00:00:00', 0), \
('Energy-98', '', 'http://listen.181fm.com/181-energy98_128k.mp3', 'http://www.energy981.com/', 'Energy-98.png', 0, 0, 0, 1, '2011-11-11', 14263, 0, '2017-01-01 00:00:00', 1), \
('Waleed', '', 'http://www.energy981.com/Energy98_128.m3u', '', 'Waleed.png', 0, 0, 0, 1, '2011-11-11', 14264, 0, '2017-01-01 00:00:00', 0), \
('Zombie-Nation', '', 'http://www.energy981.com/Energy98_128.m3u', '', 'Zombie-Nation.png', 0, 0, 0, 1, '2011-11-11', 14265, 0, '2017-01-01 00:00:00', 0), \
('This-Old-Porch', '', 'http://www.energy981.com/Energy98_128.m3u', '', 'This-Old-Porch.png', 0, 0, 0, 1, '2011-11-11', 14266, 0, '2017-01-01 00:00:00', 0), \
('RQQ-971', '', 'mms://live.cumulusstreaming.com/WRQQ-FM', 'http://www.971rqq.com/', 'RQQ-971.png', 0, 0, 0, 1, '2011-11-11', 14267, 0, '2017-01-01 00:00:00', 0), \
('96-ROCK-965', '', 'mms://live.cumulusstreaming.com/WFTK-FM', 'http://purerock965.com/', '96-ROCK-965.png', 0, 0, 0, 1, '2011-11-11', 14268, 0, '2017-01-01 00:00:00', 0);";



const char *radio_station_setupsql45="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('NovaNation', '', 'mms://live.cumulusstreaming.com/WFTK-FM', 'http://www.novanation.com.au/', 'NovaNation.png', 0, 0, 0, 1, '2011-11-11', 14269, 0, '2017-01-01 00:00:00', 0), \
('Eisradio', 'Eisradio is a hockey radio. We report live from the games of the EV Regensburg with the latest scores from other leagues in Germany.', 'http://s1.eisradio.de:8000/DSL_MP3.mp3', 'http://www.eisradio.de/', 'Eisradio.png', 0, 0, 0, 1, '2011-11-11', 14270, 0, '2017-01-01 00:00:00', 0), \
('Voivod', '', 'http://stream.laut.fm/eisradio', '', 'Voivod.png', 0, 0, 0, 1, '2011-11-11', 14271, 0, '2017-01-01 00:00:00', 0), \
('Riverwalk-Jazz', '', 'http://s1.eisradio.de:8000/DSL_MP3.mp3', '', 'Riverwalk-Jazz.png', 0, 0, 0, 1, '2011-11-11', 14272, 0, '2017-01-01 00:00:00', 0), \
('KSHK-1033', '', 'http://s1.eisradio.de:8000/DSL_MP3.mp3', 'http://www.kongradio.com/', 'KSHK-1033.png', 0, 0, 0, 1, '2011-11-11', 14273, 0, '2017-01-01 00:00:00', 0), \
('The-OverDrive', '', 'http://s1.eisradio.de:8000/DSL_MP3.mp3', '', 'The-OverDrive.png', 0, 0, 0, 1, '2011-11-11', 14274, 0, '2017-01-01 00:00:00', 0), \
('DI-Minimal', 'Finest selection of Minimal Techno &amp; House!!', 'http://listen.di.fm/partner_mp3/minimal.pls', 'http://www.di.fm/minimal', 'DI-Minimal.png', 0, 0, 0, 1, '2011-11-11', 14275, 0, '2017-01-01 00:00:00', 0), \
('Gay-FM', '', 'http://www.gayfm.de/listen.pls', 'http://www.gayfm.de/', 'Gay-FM.png', 0, 0, 0, 1, '2011-11-11', 14276, 0, '2017-01-01 00:00:00', 0), \
('ESPN-The-Jayski-Podcast', '', 'http://www.gayfm.de/listen.pls', '', 'ESPN-The-Jayski-Podcast.png', 0, 0, 0, 1, '2011-11-11', 14277, 0, '2017-01-01 00:00:00', 0), \
('KKZX-989', '', 'http://www.gayfm.de/listen.pls', 'http://www.kkzx.com/', 'KKZX-989.png', 0, 0, 0, 1, '2011-11-11', 14278, 0, '2017-01-01 00:00:00', 0), \
('KICE-940', '', 'http://www.gayfm.de/listen.pls', '', 'KICE-940.png', 0, 0, 0, 1, '2011-11-11', 14279, 0, '2017-01-01 00:00:00', 0), \
('The-Fan-610-WFNZ', 'Sports Radio The Fan 610 AM was born August 18, 1997. Since then, we have grown from a tiny voice to Charlotte&#39;s Sports leader. <BR><BR>Sports Radio The Fan 610 AM is a 5,000w (day) 1,000w (night)', 'http://radio.nrj.net/finland', 'http://www.wfnz.com/', 'The-Fan-610-WFNZ.png', 0, 0, 0, 1, '2011-11-11', 14285, 0, '2017-01-01 00:00:00', 0), \
('KDKK-975', '', 'mms://nick9.surfernetwork.com/KDKK', 'http://www.kkradionetwork.com', 'KDKK-975.png', 0, 0, 0, 1, '2011-11-11', 14289, 0, '2017-01-01 00:00:00', 0), \
('Fox-Sports-Radio---1490-The-Score', '', 'http://icy2.abacast.com/brooke-kskrammp3-32.m3u', 'http://www.541radio.com/pages/5524065.php?', 'Fox-Sports-Radio---1490-The-Score.png', 0, 0, 0, 1, '2011-11-11', 14290, 0, '2017-01-01 00:00:00', 0), \
('Real-Radio-Wales-1054', '', 'http://streaming.gmgradio.com/realradiowales.m3u', 'http://www.realradiowales.co.uk/', 'Real-Radio-Wales-1054.png', 0, 0, 0, 1, '2011-11-11', 14291, 0, '2017-01-01 00:00:00', 0), \
('WSBA-910', '', 'mms://live.cumulusstreaming.com/WSBA-AM', 'http://www.wsba910.com/', 'WSBA-910.png', 0, 0, 0, 1, '2011-11-11', 14292, 0, '2017-01-01 00:00:00', 0), \
('Kat-Country-1230', 'Bell County&#39;s #1 country music station playing today&#39;s hot country and the legends. Listen to win great prizes, be informed with latest news, weather and more.', 'http://shoutatme.com:8300', 'http://www.1230wano.com/', 'Kat-Country-1230.png', 0, 0, 0, 1, '2011-11-11', 14293, 0, '2017-01-01 00:00:00', 0), \
('King-Diamond', '', 'http://shoutatme.com:8300', '', 'King-Diamond.png', 0, 0, 0, 1, '2011-11-11', 14294, 0, '2017-01-01 00:00:00', 0), \
('WRMB-893', '', 'http://209.62.16.60:80/wrmb.mp3', 'http://www.moodyradiosouthflorida.fm/', 'WRMB-893.png', 0, 0, 0, 1, '2011-11-11', 14295, 0, '2017-01-01 00:00:00', 0), \
('KKLE-1550', '', 'http://69.24.212.14:8080/', 'http://www.kleyam.com/', 'KKLE-1550.png', 0, 0, 0, 1, '2011-11-11', 14296, 0, '2017-01-01 00:00:00', 0), \
('Hot-1079', '', 'http://69.24.212.14:8080/', 'http://hot1079philly.com', 'Hot-1079.png', 0, 0, 0, 1, '2011-11-11', 14297, 0, '2017-01-01 00:00:00', 0), \
('WYPR-HD2-881', '', 'http://live.str3am.com:9180/wypr-hd2', 'http://www.wypr.org/', 'WYPR-HD2-881.png', 0, 0, 0, 1, '2011-11-11', 14298, 0, '2017-01-01 00:00:00', 0), \
('John-Lee-Hooker', '', 'http://live.str3am.com:9180/wypr-hd2', '', 'John-Lee-Hooker.png', 0, 0, 0, 1, '2011-11-11', 14299, 0, '2017-01-01 00:00:00', 0), \
('Nephew', '', 'http://live.str3am.com:9180/wypr-hd2', '', 'Nephew.png', 0, 0, 0, 1, '2011-11-11', 14300, 0, '2017-01-01 00:00:00', 0), \
('The-Eagle-1069', '', 'http://live.str3am.com:9180/wypr-hd2', 'http://birminghamseagle.com/', 'The-Eagle-1069.png', 0, 0, 0, 1, '2011-11-11', 14301, 0, '2017-01-01 00:00:00', 0), \
('WSRW-1590', '', 'http://live.str3am.com:9180/wypr-hd2', 'http://www.wsrwam.com/', 'WSRW-1590.png', 0, 0, 0, 1, '2011-11-11', 14302, 0, '2017-01-01 00:00:00', 0), \
('Real-Radio-XS-Manchester-1061', 'Kick start your day with Sweens and Dewsbury, get the blood pumping with 30 minutes of classic rock non-stop every hour through the day, then its afternoons with Moose and Miss Rachel. Plus, each week', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', 'http://www.realradioxs.co.uk/', 'Real-Radio-XS-Manchester-1061.png', 0, 0, 0, 1, '2011-11-11', 14303, 0, '2017-01-01 00:00:00', 0), \
('WPGY-1580', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'WPGY-1580.png', 0, 0, 0, 1, '2011-11-11', 14304, 0, '2017-01-01 00:00:00', 0), \
('The-Legal-Alert', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'The-Legal-Alert.png', 0, 0, 0, 1, '2011-11-11', 14305, 0, '2017-01-01 00:00:00', 0), \
('Radioestadio-del-Motor', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Radioestadio-del-Motor.png', 0, 0, 0, 1, '2011-11-11', 14306, 0, '2017-01-01 00:00:00', 0), \
('Chris-Young', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Chris-Young.png', 0, 0, 0, 1, '2011-11-11', 14307, 0, '2017-01-01 00:00:00', 0), \
('Taake', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Taake.png', 0, 0, 0, 1, '2011-11-11', 14308, 0, '2017-01-01 00:00:00', 0), \
('Coolio', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Coolio.png', 0, 0, 0, 1, '2011-11-11', 14309, 0, '2017-01-01 00:00:00', 0), \
('Ella-Fitzgerald--Louis-Armstrong', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Ella-Fitzgerald--Louis-Armstrong.png', 0, 0, 0, 1, '2011-11-11', 14310, 0, '2017-01-01 00:00:00', 0), \
('Artists-of-the-Hour', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Artists-of-the-Hour.png', 0, 0, 0, 1, '2011-11-11', 14311, 0, '2017-01-01 00:00:00', 0), \
('Rickie-Lee-Jones', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Rickie-Lee-Jones.png', 0, 0, 0, 1, '2011-11-11', 14312, 0, '2017-01-01 00:00:00', 0), \
('Middle-Class-Rut', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Middle-Class-Rut.png', 0, 0, 0, 1, '2011-11-11', 14313, 0, '2017-01-01 00:00:00', 0), \
('Inna', '', 'http://streaming.gmgradio.com/rockradiomanchester.m3u', '', 'Inna.png', 0, 0, 0, 1, '2011-11-11', 14314, 0, '2017-01-01 00:00:00', 0), \
('bigFM-RB--Latin', 'R&amp;B, Reggae, Rap und Dancehall', 'http://srv01.bigstreams.de/stream06-64.m3u', 'http://www.mybigfm.de/', 'bigFM-RB--Latin.png', 0, 0, 0, 1, '2011-11-11', 14315, 0, '2017-01-01 00:00:00', 0), \
('Santa-Rosa-Department-of-Emergency-Services', '', 'http://relay.radioreference.com:80/ca_sonoma', 'http://www.sonoma-county.org/des/fire_service.htm', 'Santa-Rosa-Department-of-Emergency-Services.png', 0, 0, 0, 1, '2011-11-11', 14316, 0, '2017-01-01 00:00:00', 0), \
('Wren-Golden-Hurricanes-at-Woodmont-Oct-7-2011', '', 'http://relay.radioreference.com:80/ca_sonoma', '', 'Wren-Golden-Hurricanes-at-Woodmont-Oct-7-2011.png', 0, 0, 0, 1, '2011-11-11', 14317, 0, '2017-01-01 00:00:00', 0), \
('KHNR-690', '', 'http://relay.radioreference.com:80/ca_sonoma', 'http://www.khnr.com', 'KHNR-690.png', 0, 0, 0, 1, '2011-11-11', 14318, 0, '2017-01-01 00:00:00', 0), \
('Sous-les-couvertures', '', 'http://relay.radioreference.com:80/ca_sonoma', '', 'Sous-les-couvertures.png', 0, 0, 0, 1, '2011-11-11', 14319, 0, '2017-01-01 00:00:00', 0), \
('Radio-Deejay', '', 'http://live.radiodeejay.hr:7002', 'http://www.radiodeejay.hr/', 'Radio-Deejay.png', 0, 0, 0, 1, '2011-11-11', 14324, 0, '2017-01-01 00:00:00', 1), \
('The-Bull-1057', '', 'http://www.accuradio.com/shoutcast/links/hitkast.pls', 'http://www.1057thebull.com/', 'The-Bull-1057.png', 0, 0, 0, 1, '2011-11-11', 14330, 0, '2017-01-01 00:00:00', 0), \
('The-KHOP-Weekend-Rewind', '', 'hhttp://d1qg6pckcqcdk0.cloudfront.net/chr/coldplay_ha200821_02_vivalavida.m4a', '', 'The-KHOP-Weekend-Rewind.png', 0, 0, 0, 1, '2011-11-11', 14331, 0, '2017-01-01 00:00:00', 0), \
('Radio-Manhattan', '', 'http://www.radionylive.com/listen/mp3', 'http://www.radiomanhattan.pl/', 'Radio-Manhattan-998.png', 0, 0, 0, 1, '2011-11-11', 14332, 0, '2017-01-01 00:00:00', 1), \
('Since-October', '', 'http://sc.freecast.pl:8048/', '', 'Since-October.png', 0, 0, 0, 1, '2011-11-11', 14337, 0, '2017-01-01 00:00:00', 0), \
('2WD-1013', '', 'http://sc.freecast.pl:8048/', 'http://www.2wd.com/', '2WD-1013.png', 0, 0, 0, 1, '2011-11-11', 14338, 0, '2017-01-01 00:00:00', 0), \
('WSDS-1480', 'Es la &#250;nica radio en idioma Espa&#241;ol que transmite 24/7 en el &#225;rea metropolitana de Detroit, Michigan con 10 a&#241;os de experiencia en el mercado', 'http://69.162.66.178:5038', 'http://www.explosiva1480.com', 'WSDS-1480.png', 0, 0, 0, 1, '2011-11-11', 14343, 0, '2017-01-01 00:00:00', 0), \
('Money-and-the-People-You-Love', '', 'http://69.162.66.178:5038', '', 'Money-and-the-People-You-Love.png', 0, 0, 0, 1, '2011-11-11', 14344, 0, '2017-01-01 00:00:00', 0), \
('Edwin-McCain', '', 'http://69.162.66.178:5038', '', 'Edwin-McCain.png', 0, 0, 0, 1, '2011-11-11', 14345, 0, '2017-01-01 00:00:00', 0), \
('Hallam-FM-974', '', 'http://69.162.66.178:5038', 'http://www.hallamfm.co.uk/', 'Hallam-FM-974.png', 0, 0, 0, 1, '2011-11-11', 14346, 0, '2017-01-01 00:00:00', 0), \
('Chroma-Radio-Top40', '', 'http://s2.netstreaming.gr/radio/tunein.php/chr_top40/playlist.pls', 'http://www.chromaradio.net/', 'Chroma-Radio-Top40.png', 0, 0, 0, 1, '2011-11-11', 14347, 0, '2017-01-01 00:00:00', 0), \
('COOL-93-Fahrenheit-930', '', 'http://203.150.224.142:8000/', 'http://www.cool93.net/', 'COOL-93-Fahrenheit-930.png', 0, 0, 0, 1, '2011-11-11', 14349, 0, '2017-01-01 00:00:00', 0), \
('Discofox-Radio', 'Hier gibt es Deutsche und Internationale Discofox-Scheiben.', 'http://stream.laut.fm/discofox', 'http://www.dj-willy.de/', 'Discofox-Radio.png', 0, 0, 0, 1, '2011-11-11', 14350, 0, '2017-01-01 00:00:00', 0), \
('WTKS-1290', '', 'http://stream.laut.fm/discofox', 'http://www.newsradio1290wtks.com/', 'WTKS-1290.png', 0, 0, 0, 1, '2011-11-11', 14351, 0, '2017-01-01 00:00:00', 0), \
('NewsRadio-WTAX-1240', '', 'http://stream.laut.fm/discofox', 'http://www.wtax.com/', 'NewsRadio-WTAX-1240.png', 0, 0, 0, 1, '2011-11-11', 14352, 0, '2017-01-01 00:00:00', 0), \
('Young-Nef', '', 'http://stream.laut.fm/discofox', '', 'Young-Nef.png', 0, 0, 0, 1, '2011-11-11', 14353, 0, '2017-01-01 00:00:00', 0), \
('Lite-Rock-921', '', 'mms://nick9.surfernetwork.com/WEZY', 'http://www.literock921.com/', 'Lite-Rock-921.png', 0, 0, 0, 1, '2011-11-11', 14354, 0, '2017-01-01 00:00:00', 0), \
('KUPA-1370', '', 'mms://nick9.surfernetwork.com/WEZY', '', 'KUPA-1370.png', 0, 0, 0, 1, '2011-11-11', 14355, 0, '2017-01-01 00:00:00', 0), \
('KISS-FM-1023', '', 'mms://nick9.surfernetwork.com/WEZY', 'http://www.1023kissfm.com/', 'KISS-FM-1023.png', 0, 0, 0, 1, '2011-11-11', 14356, 0, '2017-01-01 00:00:00', 0), \
('News-957', 'News95.7', 'mms://nick9.surfernetwork.com/WEZY', 'http://www.news957.com/', 'News-957.png', 0, 0, 0, 1, '2011-11-11', 14357, 0, '2017-01-01 00:00:00', 0), \
('Baby-D', '', 'mms://nick9.surfernetwork.com/WEZY', '', 'Baby-D.png', 0, 0, 0, 1, '2011-11-11', 14358, 0, '2017-01-01 00:00:00', 0), \
('The-Wolf-1077', 'Real Country Variety less repetition of Top 40 more Greatest Hits!', 'http://ice3.securenetsystems.net:80/WPFX', 'http://www.1077wolf.com/', 'The-Wolf-1077.png', 0, 0, 0, 1, '2011-11-11', 14359, 0, '2017-01-01 00:00:00', 0), \
('HBR1.COM', 'House music 24/7', 'http://ubuntu.hbr1.com:19800/tronic.ogg', 'http://ubuntu.hbr1.com:19800', '', 1, 320, 62, 1, '2012-07-28', 14360, 45, '2012-09-15 22:20:34', 1)";

const char *radio_station_setupsql46="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR P5', '', 'http://live-icy.gss.dr.dk:8000/A/A25H.mp3.m3u', 'http://www.bgradio.bg/', '', 0, 0, 0, 1, '2017-01-01', 14362, 0, '2017-01-01 00:00:00', 1), \
('Radio24syv', '', 'http://rrr.sz.xlcdn.com/?account=Radio24syv&file=ENC1_Web128&type=live&service=icecast&port=8000&output=pls', 'radio24syv', '', 0, 0, 0, 1, '2017-01-01', 14363, 0, '2017-01-01 00:00:00', 1), \
('Radio Soft', '', 'http://onair.100fmlive.dk/soft_live.mp3.m3u', 'http://www.radioplay.dk/radiosoft', '', 0, 0, 0, 1, '2017-01-01', 14364, 0, '2017-01-01 00:00:00', 1), \
('Tiger FM', '', 'http://85.202.67.163:8000/stream.mp3.m3u', 'http://www.tigerfm.dk/', '', 0, 0, 0, 1, '2017-01-01', 14365, 0, '2017-01-01 00:00:00', 1), \
('Næstved Lokal Radio', 'http://78.47.115.218:8007/stream.m3u', 'http://næstvedlokalradio.dk/', 'http://www.bgradio.bg/', '', 0, 0, 0, 1, '2017-01-01', 14366, 0, '2017-01-01 00:00:00', 1), \
('PARTY VIBE', 'TECHNO-HOUSE-TRANCE','http://107.182.233.214:8046/stream', 'https://www.partyvibe.com', '', 0, 0, 0, 1, '2017-01-01', 14367, 0, '2017-01-01 00:00:00', 1), \
('Chillkyway', 'TECHNO','http://136.243.16.19:8000/stream/3/', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14368 , 0, '2017-01-01 00:00:00', 1), \
('Audiophile Jazz', 'JAZZ','http://8.38.78.173:8210/stream', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14369 , 0, '2017-01-01 00:00:00', 1), \
('Cosmic Radio Chillout', 'Trance','http://38.109.219.207:8020/', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14370 , 0, '2017-01-01 00:00:00', 1), \
('Trancemission', 'Chillout','http://uk2.internet-radio.com:31491/', 'http://www.chillkyway.net', '', 4, 0, 0, 1, '2017-01-01',14370 , 0, '2017-01-01 00:00:00', 1)";

// ('Chillkyway', 'TECHNO','http://107.182.233.214:8046/stream', 'http://www.chillkyway.net', '', 0, 0, 0, 1, '2017-01-01',14368 , 0, '2017-01-01 00:00:00', 1)";



// operet databaser/tables if now exist
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
            printf("Mysql error %d %s\n",dbname,mysql_errno(conn),mysql_error(conn));
            exit(-1);
        }
    }
    if ((mysqlerror==1049) || (mysqlerror==2006)) {
        // create db
        if (conn==NULL) conn=mysql_init(NULL);
        mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, "mysql", 0, NULL, 0);
        mysql_query(conn,"CREATE DATABASE IF NOT EXISTS mythtvcontroller");                   // create radio stations db
        res = mysql_store_result(conn);
        printf("Create db %s Mysql error %d \n",dbname,mysql_errno(conn));
        mysql_query(conn,"use mythtvcontroller");
        res = mysql_store_result(conn);
        printf("On select mythtvcontroller Mysql error %d \n",mysql_errno(conn));
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
            mysql_query(conn,radio_station_setupsql34);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql34);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql35);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql35);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql36);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql36);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql37);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql37);
              exit(1);
            }
            mysql_query(conn,radio_station_setupsql38);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d\n" , mysqlerror);
              printf("%s\n" , radio_station_setupsql38);
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
            mysql_query(conn,radio_station_setupsql46);
            res = mysql_store_result(conn);
            mysqlerror=mysql_errno(conn);
            if (mysqlerror) {
              printf("%d" , mysqlerror);
              printf("%s\n" , radio_station_setupsql46);
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
        printf("Mysql error %d %s\n",dbname,mysql_errno(conn),mysql_error(conn));
        exit(1);
    }
    return(0);
}
