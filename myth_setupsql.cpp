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
                                                                                    ('Trance',            29, 29)";





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

// select to db the start
// select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by intnr
//

const char *radio_station_setupsql="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('P1 DK', 'Danish radio play clasisk music [Bitrate: 128]', 'http://live-icy.gss.dr.dk:8000/A/A03H.mp3', '', 'DR-P1-908.png', 8, 320, 63, 1, '2011-07-29', 1, 45, '2011-09-16 21:50:41', 1), \
('P2 DK', 'Dansk radio', 'http://live-icy.gss.dr.dk:8000/A/A04H.mp3', 'www.dr.dk/P2', 'DR-P2-1023.png', 1, 320, 62, 1, '2011-07-28', 2, 45, '2011-09-15 22:20:34', 1), \
('P3 DK', 'www.p3.dk Dansk radio.', 'http://live-icy.gss.dr.dk:8000/A/A05H.mp3', 'www.dr.dk', 'DR-P3-939.png', 1, 128, 77, 1, '2011-07-28', 3, 45, '2011-10-20 15:37:44', 1), \
('DR P7 Mix DK', '[Bitrate: 128]', 'http://live-icy.gss.dr.dk:8000/A/A21H.mp3', '', 'DR-P7-Mix.png', 40, 320, 64, 1, '2011-07-30', 4, 45, '2011-09-29 13:40:49', 1), \
('psyradio*fm', '[Bitrate: 128]', 'http://81.88.36.42:8020', '', '', 41, 128, 0, 1, '2011-08-11', 5, 8, '2011-10-20 15:37:44', 1), \
('MAXXIMA', '', 'http://maxxima.mine.nu:8000/', '', 'Maxxima.png', 12, 0, 10, 1, '2011-08-02', 6, 4, '2011-10-02 02:25:35', 1), \
('Bay Radio', '[Bitrate: 128]', 'http://icy-e-01.sharp-stream.com/tcbay.mp3', 'http://www.swanseabayradio.com/', '', 0, 320, 1, 1, '2011-07-31', 7, 45, '2011-09-17 18:06:22', 1), \
('Heat radio UK', '[Bitrate: 128] Denne radio er mega cool og spiller meget godt og blandet musik fra england. ', 'http://ams01.egihosting.com:9108/', '', 'Heat-Radio.png', 3, 128, 242, 1, '2011-07-31', 9, 9, '2011-10-21 15:23:52', 1),  \
('Radio 100', 'Med en god og varieret musikprofil, morsomme og intelligente værter samt en seriøs nyhedsformidling har Radio 100 leveret kvalitetsradio til de danske radiolyttere hele døgnet siden den 15. november 2', 'http://onair.100fmlive.dk/100fm_live.mp3', '', '', 14, 128, 43, 1, '2011-07-31', 11, 45, '2011-10-09 16:13:11', 1), \
('Radio Aura DK', 'DK', 'http://icecast.xstream.dk:80/aura', '', 'Radio-Aura-1054.png', 40, 128, 46, 1, '2011-08-02', 12, 45, '2011-10-11 23:05:02', 1), \
('181FM', '', 'http://uplink.181.fm:8068/', '', '181.fm.png', 0, 128, 1, 1, '2011-08-02', 15, 7, '2011-09-27 17:58:45', 1), \
('Energy 93 Eurodance', '', 'http://uplink.181.fm:8044/', '', '181.fm.png', 4, 0, 5, 1, '2011-08-02', 16, 7, '2011-10-07 16:02:00', 1), \
('Just Hiphop', '', 'http://stream.laut.fm:80/justhiphop', '', '', 5, 0, 2, 1, '2011-08-02', 17, 29, '2011-09-13 10:53:29', 1), \
('Groove Salad', 'A nicely chilled plate of ambient/downtempo beats and grooves.', 'http://ice1.somafm.com/groovesalad-128-aac', '', '', 16, 128, 13, 1, '2011-07-31', 19, 7, '2011-10-07 15:58:57', 1), \
('Radio EFM', 'France', 'http://radioefm.ice.infomaniak.ch:80/radioefm-high.mp3', '', '', 0, 128, 6, 1, '2011-08-01', 22, 4, NULL, 1), \
('The voice DK', 'The voice denmark.', 'http://195.184.101.203/voice128', '', 'The-Voice-1049.png', 40, 64, 5, 1, '2011-08-01', 23, 45, '2011-09-11 00:08:02', 1), \
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
('psyradio 2*fm', '[Bitrate: 128]', 'http://81.88.36.42:8010', '', '', 0, 128, 0, 1, '2011-08-11', 2098, 8, NULL, 1), \
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
('psyradio 3*fm', '[Bitrate: 128]', 'http://81.88.36.42:8010', '', '', 0, 128, 0, 1, '2011-08-11', 4105, 8, NULL, 1), \
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
('psyradio 4*fm', 'chillchannel [Bitrate: 128] [Max Listeners: 410]', 'http://81.88.36.42:8020', '', '', 0, 128, 0, 1, '2011-08-11', 4757, 8, NULL, 1), \
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
('Radio-Ballerup-902', '', 'http://96.31.83.86:8200/', 'http://www.radioballerup.com/', 'Radio-Ballerup-902.png', 0, 0, 0, 1, '2011-11-11', 10461, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Esbjerg', '', 'http://live-icy.gss.dr.dk:8000/A/A15H.mp3', 'http://www.dr.dk/Regioner/Esbjerg/', 'DR-P4-Esbjerg-990.png', 0, 0, 0, 1, '2011-11-11', 11233, 0, '2017-01-01 00:00:00', 1), \
('DR Nyheder', '', 'http://live-icy.gss.dr.dk:8000/A/A02H.mp3', 'http://www.dr.dk/radio/alle_kanaler/nyheder.asp', 'DR-Nyheder.png', 0, 0, 0, 1, '2011-11-11', 11396, 0, '2017-01-01 00:00:00', 1), \
('DR P4 Trekanten', '', 'http://live-icy.gss.dr.dk:8000/A/A13H.mp3', 'http://www.dr.dk/p4/trekanten/', 'DR-P4-Trekanten-940.png', 0, 0, 0, 1, '2011-11-11', 11450, 0, '2017-01-01 00:00:00', 1), \
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

const char *radio_station_setupsql39="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR P1', '', 'http://live-icy.gss.dr.dk:8000/A/A03H.mp3', 'http://www.dr.dk/p1/', 'DR-P1-908.png', 0, 0, 0, 1, '2011-11-11', 12822, 0, '2017-01-01 00:00:00', 1);";

const char *radio_station_setupsql40="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('DR P4 København', '', 'http://live-icy.gss.dr.dk:8000/A/A08H.mp3', 'http://www.dr.dk/p4/kbh/', 'DR-P4-Koebenhavn-965.png', 0, 0, 0, 1, '2011-11-11', 13160, 0, '2017-01-01 00:00:00', 1), \
('Spirit-1059', 'SPIRIT 105.9, The Spirit of Austin, features Adult Contemporary Christian music; a popular music format that appeals to a wide range of listeners.', 'http://crista-kfmk.streamguys1.com/kfmkaacp.m3u', 'http://www.spirit1059.com/', 'Spirit-1059.png', 0, 0, 0, 1, '2011-11-11', 13176, 0, '2017-01-01 00:00:00', 1), \
('Dance90s', '', 'http://listen.radionomy.com/dance90', 'http://dance90.tk/', 'Dance90s.png', 0, 0, 0, 1, '2011-11-11', 13185, 0, '2017-01-01 00:00:00', 1), \
('WESC-FM-925', '', 'http://listen.radionomy.com/dance90', 'http://www.wescfm.com/', 'WESC-FM-925.png', 0, 0, 0, 1, '2011-11-11', 13186, 0, '2017-01-01 00:00:00', 1), \
('X1039', '', 'http://www.181.fm/winamp.pls?station=181-beatles&style=&description=Beatles', 'http://www.x1039.com/', 'X1039.png', 0, 0, 0, 1, '2011-11-11', 13189, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql41="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('The-Beat-1017', '', 'http://asx.abacast.com/longisland-wbeafm-64.pls', 'http://www.1017thebeat.com/', 'The-Beat-1017.png', 0, 0, 0, 1, '2011-11-11', 13192, 0, '2017-01-01 00:00:00', 1), \
('WNXT-1260', 'Today much of WNXT is dominated by ESPN Radio and local sports. The station dumped much of its country music in 2004 for ESPN Radio. The station has a major variety of both local and professional spor', 'http://asx.abacast.com/longisland-wbeafm-64.pls', 'http://www.angelfire.com/oh3/wnxt/wnxtam.html', 'WNXT-1260.png', 0, 0, 0, 1, '2011-11-11', 13193, 0, '2017-01-01 00:00:00', 1), \
('The-Voice-720', '', 'http://asx.abacast.com/longisland-wbeafm-64.pls', 'http://www.newsradio720.com/', 'The-Voice-720.png', 0, 0, 0, 1, '2011-11-11', 13194, 0, '2017-01-01 00:00:00', 1), \
('DIE-NEUE-1077', '', 'http://edge.live.mp3.mdn.newmedia.nacamar.net/ps-dieneue_rock/livestream_hi.mp3', 'http://www.dieneue1077.de/', 'DIE-NEUE-1077.png', 0, 0, 0, 1, '2011-11-11', 13195, 0, '2017-01-01 00:00:00', 1), \
('Classic-Jenny-FM', '', 'http://85.214.52.25:8500/', 'http://classic.jenny.fm/', 'Classic-Jenny-FM.png', 0, 0, 0, 1, '2011-11-11', 13226, 0, '2017-01-01 00:00:00', 1), \
('Dallas-Stars-at-Anaheim-Ducks-Oct-21-2011', '', 'http://85.214.52.25:8500/', '', '', 0, 0, 0, 1, '2011-11-11', 13227, 0, '2017-01-01 00:00:00', 1), \
('Fox-Sports-Memphis-730', '', 'http://85.214.52.25:8500/', 'http://www.730foxsports.com/', 'Fox-Sports-Memphis-730.png', 0, 0, 0, 1, '2011-11-11', 13228, 0, '2017-01-01 00:00:00', 1), \
('Salt-and-Light-Catholic-Radio-1140', '', 'http://salt-light-stream-01.miriamtech.net:8000/live', 'http://www.saltandlightradio.com/', 'Salt-and-Light-Catholic-Radio-1140.png', 0, 0, 0, 1, '2011-11-11', 13230, 0, '2017-01-01 00:00:00', 1), \
('4KQ-693', '', 'http://icecast.arn.com.au/4kq.aac.m3u', 'http://www.4kq.com.au/', '4KQ-693.png', 0, 0, 0, 1, '2011-11-11', 13235, 0, '2017-01-01 00:00:00', 1), \
('Motley-Fool-Money-Radio-Show', '', 'http://icecast.arn.com.au/4kq.aac.m3u', '', 'Motley-Fool-Money-Radio-Show.png', 0, 0, 0, 1, '2011-11-11', 13236, 0, '2017-01-01 00:00:00', 1), \
('WWJ-Newsradio-950', 'WWJ Newsradio 950 is Detroit&#39;s award winning all-news radio station and is live, local and committed to providing metro Detroiters the place to turn for up-to-the-minute news, traffic, sports, bus', 'http://icecast.arn.com.au/4kq.aac.m3u', 'http://www.wwj.com/', 'WWJ-Newsradio-950.png', 0, 0, 0, 1, '2011-11-11', 13237, 0, '2017-01-01 00:00:00', 1), \
('Eurodance-90-Radio', '', 'http://listen.radionomy.com/eurodance-90', 'http://eurodance90.fr/', 'Eurodance-90-Radio.png', 0, 0, 0, 1, '2011-11-11', 13276, 0, '2017-01-01 00:00:00', 1), \
('Lite-Rock-973', '', 'http://listen.radionomy.com/eurodance-90', 'http://www.literock973.com/', 'Lite-Rock-973.png', 0, 0, 0, 1, '2011-11-11', 13277, 0, '2017-01-01 00:00:00', 1), \
('Mix-929', '', 'http://listen.radionomy.com/eurodance-90', 'http://www.mymix929.com/', 'Mix-929.png', 0, 0, 0, 1, '2011-11-11', 13278, 0, '2017-01-01 00:00:00', 1), \
('Agoria', '', 'http://174.120.124.178:7110/', '', 'Agoria.png', 0, 0, 0, 1, '2011-11-11', 13323, 0, '2017-01-01 00:00:00', 1), \
('Italian-Graffiati', 'In diretta live da Montr&#233;al (Qu&#233;bec)<BR>Canada, per tutti gli italiani nel mondo trasmettiamo le piu&#39; belle selezioni musicali dei favolosi anni  &#39;60 e &#39;70.<BR>La grande musica i', 'http://ibiza.broadstreamer.com:8000', 'http://www.italiangraffiati.com/', 'Italian-Graffiati.png', 0, 0, 0, 1, '2011-11-11', 13327, 0, '2017-01-01 00:00:00', 1), \
('Mas-FM-941', '', 'http://174.122.121.106:8000/mas94-ogg.ogg', 'http://www.cincoradio.com.mx/emisoras.asp?emi=XHJE', 'Mas-FM-941.png', 0, 0, 0, 1, '2011-11-11', 13372, 0, '2017-01-01 00:00:00', 1), \
('WEBY-1330', '', 'http://mega5.radioserver.co.uk:8026', 'http://www.1330weby.com/', 'WEBY-1330.png', 0, 0, 0, 1, '2011-11-11', 13374, 0, '2017-01-01 00:00:00', 1), \
('Streekradio-1048', '', 'http://livestream.streekradio.com/live', 'http://www.streekradio.com/', 'Streekradio-1048.png', 0, 0, 0, 1, '2011-11-11', 13375, 0, '2017-01-01 00:00:00', 1), \
('Radio-Metro-1024', '', 'http://195.182.132.18:8230', 'http://radiometro.ru/', 'Radio-Metro-1024.png', 0, 0, 0, 1, '2011-11-11', 13418, 0, '2017-01-01 00:00:00', 1), \
('Fun-Radio-Top-20', '24-hour stream 20 songs fun radio.', 'http://stream.funradio.sk:8000/top20128.mp3', 'http://www.funradio.sk/', 'Fun-Radio-Top-20.png', 0, 0, 0, 1, '2011-11-11', 13447, 0, '2017-01-01 00:00:00', 1), \
('The-Sheep-540', '', 'http://wgth.serverroom.us:8652', 'http://www.wgth.net/', 'The-Sheep-540.png', 0, 0, 0, 1, '2011-11-11', 13448, 0, '2017-01-01 00:00:00', 1), \
('DI-Lounge', 'Sit back and enjoy the lounge grooves!', 'http://listen.di.fm/partner_mp3/lounge.pls', 'http://www.di.fm/lounge', 'DI-Lounge.png', 0, 0, 0, 1, '2011-11-11', 13460, 0, '2017-01-01 00:00:00', 1), \
('SportsLine-Radio-Network', '', 'mms://nick9.surfernetwork.com/VALRUGER', 'http://slrnradiosports.com/', 'SportsLine-Radio-Network.png', 0, 0, 0, 1, '2011-11-11', 13461, 0, '2017-01-01 00:00:00', 0);";

const char *radio_station_setupsql42="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Easy-Network-987', 'Easy Network , storica emittente di Veneto e Friuli Venezia Giulia, grazie alle sua diffusione capillare e alla sua programmazione musicale', 'http://str01.fluidstream.net:6030', 'http://www.easynetwork.fm/', 'Easy-Network-987.png', 0, 0, 0, 1, '2011-11-11', 13477, 0, '2017-01-01 00:00:00', 1), \
('RFD-Illinois', '', 'http://str01.fluidstream.net:6030', '', 'RFD-Illinois.png', 0, 0, 0, 1, '2011-11-11', 13479, 0, '2017-01-01 00:00:00', 1), \
('AFR-Talk-917', 'American Family Radio (AFR) is a network of more than 180 radio stations broadcasting Christian-oriented programming to over 40 states. AFR was launched by Rev. Donald Wildmon in 1991 as a ministry of', 'http://mediaserver3.afa.net:8000/talkhigh.mp3', 'http://www.afr.net/', 'AFR-Talk-917.png', 0, 0, 0, 1, '2011-11-11', 13586, 0, '2017-01-01 00:00:00', 1), \
('Dive-Bar-Jukebox-Radio', '', 'http://listen.radionomy.com/dive-bar-jukebox', 'http://www.charlestonillustrated.com/radio.htm', 'Dive-Bar-Jukebox-Radio.png', 0, 0, 0, 1, '2011-11-11', 13598, 0, '2017-01-01 00:00:00', 1), \
('Jimmie-Rodgers', '', 'http://listen.radionomy.com/dive-bar-jukebox', '', 'Jimmie-Rodgers.png', 0, 0, 0, 1, '2011-11-11', 13599, 0, '2017-01-01 00:00:00', 1), \
('D-FM-902', '', 'http://striiming.trio.ee/dfm.mp3', 'http://www.dfm.ee/', 'D-FM-902.png', 0, 0, 0, 1, '2011-11-11', 13609, 0, '2017-01-01 00:00:00', 1), \
('Stevie-Storm', '', 'http://striiming.trio.ee/dfm.mp3', '', 'Stevie-Storm.png', 0, 0, 0, 1, '2011-11-11', 13610, 0, '2017-01-01 00:00:00', 1), \
('Radio-SLR-1010', '', 'http://netradio.radioslr.dk', 'http://www.radioslr.dk/', 'Radio-SLR-1010.png', 0, 0, 0, 1, '2011-11-11', 13617, 0, '2017-01-01 00:00:00', 1), \
('No-Holds-Barred-Radio', '', 'http://www.newerastreaming.com:8000', 'http://www.nhbradio.com/', 'No-Holds-Barred-Radio.png', 0, 0, 0, 1, '2011-11-11', 13627, 0, '2017-01-01 00:00:00', 1), \
('All-Dance-Radio', '', 'http://listen.radionomy.com/all-dance-radio', 'http://www.radionomy.com/en/radio/all-dance-radio', 'All-Dance-Radio.png', 0, 0, 1, 1, '2011-11-11', 13717, 0, '2011-10-15 20:11:45', 1), \
('Farm-Radio-1010', '', 'http://alpha.newerastreaming.com:8016', 'http://www.ksir.com/', 'Farm-Radio-1010.png', 0, 0, 0, 1, '2011-11-11', 13720, 0, '2017-01-01 00:00:00', 1), \
('Z-104-1041', '', 'http://alpha.newerastreaming.com:8016', 'http://www.z104fm.com/', 'Z-104-1041.png', 0, 0, 0, 1, '2011-11-11', 13721, 0, '2017-01-01 00:00:00', 1), \
('Fun101-FM-1011', 'Todays Hits, Five decades of hits and Classic Rock.', 'http://www.mainstreamnetwork.com/listen/dynamicasx.asp?station=wtga-fm', 'http://www.fun101fm.com', 'Fun101-FM-1011.png', 0, 0, 0, 1, '2011-11-11', 13735, 0, '2017-01-01 00:00:00', 1), \
('Juice-1072', 'Juice 107.2 is The Local Radio Station for Brighton &amp; Hove, UK. Broadcasting LIVE from North Street', 'http://tx.sharp-stream.com/icecast.php?i=juice1072.mp3', 'http://www.juicebrighton.com/', 'Juice-1072.png', 0, 0, 0, 1, '2011-11-11', 13737, 0, '2017-01-01 00:00:00', 1);";



const char *radio_station_setupsql43="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Foster-the-People', '', 'http://netradio.radioalfa.dk', '', 'Foster-the-People.png', 0, 0, 0, 1, '2011-11-11', 13750, 0, '2017-01-01 00:00:00', 1), \
('DR P3', '', 'http://live-icy.gss.dr.dk:8000/A/A05H.mp3', 'http://www.dr.dk/p3/', 'DR-P3-939.png', 0, 0, 0, 1, '2011-11-11', 13772, 0, '2017-01-01 00:00:00', 1), \
('Radio-100-1036', '', 'http://onair.100fmlive.dk/100fm_live.mp3', 'http://radio100.dk', 'Radio-100-1036.png', 0, 0, 0, 1, '2011-11-11', 13773, 0, '2017-01-01 00:00:00', 1), \
('181FM-Trance-Jazz', '', 'http://icyrelay.181.fm/181-trancejazz_128k.mp3', 'http://www.181.fm/', '181FM-Trance-Jazz.png', 0, 0, 0, 1, '2011-11-11', 13802, 0, '2017-01-01 00:00:00', 1), \
('ZaycevFM-RnB', '', 'http://www.zaycev.fm:9002/rnb/ZaycevFM(256)', 'http://www.zaycev.fm', 'ZaycevFM-RnB.png', 0, 0, 0, 1, '2011-11-11', 13907, 0, '2017-01-01 00:00:00', 1), \
('WKXL-1450', '', 'http://wkxl.serverroom.us:7516', 'http://wkxl1450.com/', 'WKXL-1450.png', 0, 0, 0, 1, '2011-11-11', 13915, 0, '2017-01-01 00:00:00', 1), \
('Old-School-Radio', 'Les classiques du funk,dela soul et du hip hop en continu sur Old Schol radio !', 'http://listen.radionomy.com/oldschool', 'http://www.oldschool-radio.com/', 'Old-School-Radio.png', 0, 0, 0, 1, '2011-11-11', 13946, 0, '2017-01-01 00:00:00', 1), \
('923-NOW', 'Welcome to New Yorks new hit music channel,92.3 Now.', 'http://tx.sharp-stream.com/icecast.php?i=original106.mp3', 'http://www.923now.com', '923-NOW.png', 0, 0, 2, 1, '2011-11-11', 13952, 0, '2011-10-14 15:57:22', 1);";




// check from here




const char *radio_station_setupsql44="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('181FM-Super-70s', '', 'http://listen.181fm.com/181-70s_128k.mp3', 'http://www.181.fm/', '181FM-Super-70s.png', 0, 0, 0, 1, '2011-11-11', 14040, 0, '2017-01-01 00:00:00', 1), \
('Talk-Radio-1270', '', 'http://pub5.radiotunes.com:80/radiotunes_urbanjamz', 'http://www.wgsv.com/', 'Talk-Radio-1270.png', 0, 0, 0, 1, '2011-11-11', 14075, 0, '2017-01-01 00:00:00', 1), \
('Slow-Jam', 'Une radio Slow Jam pour des France.', 'http://listen.radionomy.com/slow-jam', 'http://www.radionomy.com/en/radio/slow-jam', 'Slow-Jam.png', 0, 0, 0, 1, '2011-11-11', 14077, 0, '2017-01-01 00:00:00', 1), \
('Bossa-Nova-on-JAZZRADIOcom', 'Sweet sounds of bossa nova and the flavors of Brazil.', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', 'http://www.jazzradio.com/', 'Bossa-Nova-on-JAZZRADIOcom.png', 0, 0, 0, 1, '2011-11-11', 14093, 0, '2017-01-01 00:00:00', 1), \
('Jeremy-Vine', '', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', '', 'Jeremy-Vine.png', 0, 0, 0, 1, '2011-11-11', 14094, 0, '2017-01-01 00:00:00', 1), \
('Ralph-Tees-Luxury-Soul', '', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', '', 'Ralph-Tees-Luxury-Soul.png', 0, 0, 0, 1, '2011-11-11', 14095, 0, '2017-01-01 00:00:00', 1), \
('Charlie-Winston', '', 'http://listen.jazzradio.com/partner_aac/bossanova.pls', '', 'Charlie-Winston.png', 0, 0, 0, 1, '2011-11-11', 14097, 0, '2017-01-01 00:00:00', 1), \
('99-X-993', '', 'http://amber.streamguys.com:4570', 'http://www.99xwjbx.com/', '99-X-993.png', 0, 0, 1, 1, '2011-11-11', 14108, 0, '2011-10-17 23:40:45', 1), \
('DR P4 jylland', '', 'http://live-icy.gss.dr.dk:8000/A/A10H.mp3', 'http://www.dr.dk/p4/aarhus/', 'DR-P4-jylland-959.png', 0, 0, 5, 1, '2011-11-11', 14150, 0, '2011-10-14 16:08:24', 1), \
('Radio-Tunisie24 Dance', 'Among the stations Radio Tunisie24, users can discover Urban Radio Tunisie devoted to the Underground Music Rap Hip-Hop Rb.', 'http://tunisie24.net/radio-tunisie24-dance.m3u', 'http://www.tunisie24.net/', 'Radio-Tunisie24---Dance.png', 0, 0, 0, 1, '2011-11-11', 14259, 0, '2017-01-01 00:00:00', 0), \
('Energy-98', '', 'http://listen.181fm.com/181-energy98_128k.mp3', 'http://www.energy981.com/', 'Energy-98.png', 0, 0, 0, 1, '2011-11-11', 14263, 0, '2017-01-01 00:00:00', 1);";


const char *radio_station_setupsql45="REPLACE INTO `radio_stations` (`name`, `beskriv`, `stream_url`, `homepage`, `gfx_link`, `art`, `bitrate`, `popular`, `aktiv`, `createdate`, `intnr`, `landekode`, `lastplayed`, `online`) VALUES \
('Voivod', '', 'http://stream.laut.fm/eisradio', '', 'Voivod.png', 0, 0, 0, 1, '2011-11-11', 14271, 0, '2017-01-01 00:00:00', 1), \
('Radio-Deejay', '', 'http://live.radiodeejay.hr:7002', 'http://www.radiodeejay.hr/', 'Radio-Deejay.png', 0, 0, 0, 1, '2011-11-11', 14324, 0, '2017-01-01 00:00:00', 1), \
('Radio-Manhattan', '', 'http://www.radionylive.com/listen/mp3', 'http://www.radiomanhattan.pl/', 'Radio-Manhattan-998.png', 0, 0, 0, 1, '2011-11-11', 14332, 0, '2017-01-01 00:00:00', 1), \
('Discofox-Radio', 'Hier gibt es Deutsche und Internationale Discofox-Scheiben.', 'http://stream.laut.fm/discofox', 'http://www.dj-willy.de/', 'Discofox-Radio.png', 0, 0, 0, 1, '2011-11-11', 14350, 0, '2017-01-01 00:00:00', 1), \
('HBR1.COM', 'House music 24/7', 'http://ubuntu.hbr1.com:19800/tronic.ogg', 'http://ubuntu.hbr1.com:19800', '', 1, 320, 62, 1, '2012-07-28', 14360, 45, '2012-09-15 22:20:34', 1)";



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
        printf("Mysql error %d %s\n",dbname,mysql_errno(conn),mysql_error(conn));
        exit(1);
    }
    return(0);
}
