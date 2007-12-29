/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2006 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include "common.h"
#include "i18n.h"

const char *i18n_name = 0;

const tI18nPhrase Phrases[] = {
/*
 * French     translations provided by Maxime Guilbot <maxime.guilbot@orange.fr>
 *               additions provided by Pierre-Henri Beguin <ph.beguin@free.fr>
 *               additions provided by Jerome Rousset <zekje@hotmail.com>
 * Slovenian  translations provided by Matjaz Thaler <matjaz.thaler@guest.arnes.si>
 * Greek      translations provided by Dimitrios Dimitrakos <mail@dimitrios.de>
 * Swedish    translations provided by Jan Ekholm <chakie@infa.abo.fi>
 * Spanish & 
 * Catalan    translations provided by Ramon Roca <ramon.roca@xcombo.com>
 * Finnish    translations provided by Rolf Ahrenberg <rahrenbe@cc.hut.fi>
 *               additions provided by Ville Skytt� <ville.skytta@iki.fi>
 * Russian    translations provided by Vyacheslav Dikonov <sdiconov@mail.ru>
*/
  { "Parent",
    "Zur�ck",
    "Nazaj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parent",
    "", // TODO
    "Takaisin",
    "", // TODO
    "Predecesor", 
    "Piso",
    "Tillbaka",
    "", // TODO
    "", // TODO
    "Anterior",
#if APIVERSNUM >= 10302
    "�����",
#endif
  },
  { "Select",
    "Ausw�hlen",
    "Izberi",
    "", // TODO
    "", // TODO
    "", // TODO
    "S�lectionner",
    "", // TODO
    "Valitse",
    "", // TODO
    "Seleccionar", 
    "Epilogi",
    "V�lj",
    "", // TODO
    "", // TODO
    "Escollir",
#if APIVERSNUM >= 10302
    "�������",
#endif
  },
  { "Error scanning directory!",
    "Fehler beim Lesen des Verzeichnisses!",
    "Napaka pri pregledovanju direktorija!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours du r�pertoire!",
    "", // TODO
    "Hakemiston avaaminen ep�onnistui!",
    "", // TODO
    "�Error al leer una carpeta!", 
    "Lathos stin sarosi tou fakelou!",
    "Kunde inte l�sa katalogen!",
    "", // TODO
    "", // TODO
    "Error al llegir una carpeta!",
#if APIVERSNUM >= 10302
    "������ ������������ ��������!",
#endif
  },
  { "Source",
    "Datentr�ger",
    "Izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source",
    "", // TODO
    "L�hde",
    "", // TODO
    "Origen", 
    "Pigi",
    "K�lla",
    "", // TODO
    "", // TODO
    "Or�gen",
#if APIVERSNUM >= 10302
    "����",
#endif
  },
  { "Mount",
    "Einbinden",
    "Priklopi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Monter",
    "", // TODO
    "Kiinnit�",
    "", // TODO
    "Mount", 
    "Sindesi",
    "Montera",
    "", // TODO
    "", // TODO
    "Mount",
#if APIVERSNUM >= 10302
    "����������",
#endif
  },
  { "Unmount",
    "Aush�ngen",
    "Izklopi",
    "", // TODO
    "", // TODO
    "", // TODO
    "D�monter",
    "", // TODO
    "Irrota",
    "", // TODO
    "Unmount",
    "Aposindesi",
    "Avmontera",
    "", // TODO
    "", // TODO
    "Unmount",
#if APIVERSNUM >= 10302
    "���������",
#endif
  },
  { "Eject",
    "Auswerfen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Avaa",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "", // TODO
#endif
  },
  { "Selected source is not mounted!",
    "Ausgew�hlte Datentr�ger ist nicht eingebunden!",
    "Izbran izvor ni prikljucen!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source s�lectionn�e non mont�e!",
    "", // TODO
    "Valittu l�hde ei ole kiinnitetty!",
    "", // TODO
    "�El origen deseado no est� montado!", 
    "Epilegmeni Pigi den ine sindemeni!",
    "Den valda k�llan �r inte monterad!",
    "", // TODO
    "", // TODO
    "l'Origen sel.leccionat no est� muntat!",
#if APIVERSNUM >= 10302
    "��������� ���� �� ���������!",
#endif
  },
  { "Mount succeeded",
    "Einbinden erfolgreich",
    "Priklop izveden",
    "", // TODO
    "", // TODO
    "", // TODO
    "Montage r�ussi",
    "", // TODO
    "Kiinnitt�minen onnistui",
    "", // TODO
    "mount correcto", 
    "I sindesi petixe",
    "Monteringen lyckades",
    "", // TODO
    "", // TODO
    "mount correcte",
#if APIVERSNUM >= 10302
    "���� ���������",
#endif
  },
  { "Mount failed!",
    "Einbinden fehlgeschlagen!",
    "Napaka pri priklopu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du montage!",
    "", // TODO
    "Kiinnitt�minen ep�onnistui!",
    "", // TODO
    "�No he podido montar!", 
    "I sindesi apetixe!",
    "Monteringen misslyckades!",
    "", // TODO
    "", // TODO
    "No he pogut muntar!",
#if APIVERSNUM >= 10302
    "������ ����������� �����!",
#endif
  },
  { "Unmount succeeded",
    "Aush�ngen erfolgreich",
    "Izklop izveden",
    "", // TODO
    "", // TODO
    "", // TODO
    "D�montage r�ussi",
    "", // TODO
    "Irrottaminen onnistui",
    "", // TODO
    "�xito al unmount", 
    "I aposindesi itan epitixisi",
    "Avmonteringen lyckades",
    "", // TODO
    "", // TODO
    "unmount amb �xit",
#if APIVERSNUM >= 10302
    "���� ��������",
#endif
  },
  { "Unmount failed!",
    "Aush�ngen fehlgeschlagen!",
    "Napaka pri izklopu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du d�montage!",
    "", // TODO
    "Irrottaminen ep�onnistui!",
    "", // TODO
    "�No puedo desmontar!", 
    "I aposindesi den itan epitixis!",
    "Avmonteringen misslyckades!",
    "", // TODO
    "", // TODO
    "No puc desmontar!",
#if APIVERSNUM >= 10302
    "������ ���������� �����!",
#endif
  },
  { "Eject failed!",
    "Auswerfen fehlgeschlagen!",
    "Napaka pri izmetu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec de l'�jection!",
    "", // TODO
    "Aseman avaaminen ep�onnistui!",
    "", // TODO
    "�No puedo expulsar!", 
    "I apovoli apetixe!",
    "Mata ut!",
    "", // TODO
    "", // TODO
    "No puc expulsar!",
#if APIVERSNUM >= 10302
    "������ ���������� �����!",
#endif
  },
  { "Hide mainmenu entry",
    "Hauptmen�eintrag verstecken",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Cacher l entr�e du menu",
    "", // TODO
    "Piilota valinta p��valikosta",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "������ ������� � ����",
#endif
  },

// start of MP3 specific phrases

  { "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3-soitin",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
#if APIVERSNUM >= 10302
    "������������� MP3",
#endif
  },
  { "A versatile audio player",
    "Ein vielseitiger Audio-Player",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Lecteur multiformats",
    "", // TODO
    "Monipuolinen audiosoitin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "������������� �����������",
#endif
  },
  { "Browse",
    "Bl�ttern",
    "Isci",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parcourir",
    "", // TODO
    "Selaa",
    "", // TODO
    "Explorar",
    "Selida",
    "Bl�ddra",
    "", // TODO
    "", // TODO
    "Explorar",
#if APIVERSNUM >= 10302
    "��������",
#endif
  },
  { "MP3 source",
    "MP3 Datentr�ger",
    "MP3 izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source MP3",
    "", // TODO
    "MP3-l�hde",
    "", // TODO
    "Origen MP3", 
    "Pigi MP3",
    "MP3 k�lla",
    "", // TODO
    "", // TODO
    "Or�gen MP3",
#if APIVERSNUM >= 10302
    "���� MP3",
#endif
  },
  { "Rename",
    "Umbenennen",
    "Preimenuj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer",
    "", // TODO
    "Nime�",
    "", // TODO
    "Renombrar", 
    "Alagi Onomatos",
    "D�p om",
    "", // TODO
    "", // TODO
    "Reanomenar",
#if APIVERSNUM >= 10302
    "�������������",
#endif
  },
  { "Add",
    "Hinzuf�gen",
    "Dodaj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter",
    "", // TODO
    "Lis��",
    "", // TODO
    "A�adir", 
    "Prosthesi",
    "L�gg till",
    "", // TODO
    "", // TODO
    "Afegir",
#if APIVERSNUM >= 10302
    "��������",
#endif
  },
  { "Remove",
    "Entfernen",
    "Odstrani",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever",
    "", // TODO
    "Poista",
    "", // TODO
    "Borrar", 
    "Aferesi",
    "Radera",
    "", // TODO
    "", // TODO
    "Esborrar",
#if APIVERSNUM >= 10302
    "�������",
#endif
  },
  { "Play all",
    "Alle abspielen",
    "Predvajaj vse",
    "", // TODO
    "", // TODO
    "", // TODO
    "Jouer tous",
    "", // TODO
    "Soita kaikki",
    "", // TODO
    "Reproducirlo todo", 
    "Peksimo olon",
    "Spela alla",
    "", // TODO
    "", // TODO
    "Escoltar-ho tot",
#if APIVERSNUM >= 10302
    "������������� ���",
#endif
  },
  { "Add all",
    "Alle hinzuf�gen",
    "Dodaj vse",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter tous",
    "", // TODO
    "Lis�� kaikki",
    "", // TODO
    "A�adir todo", 
    "Prosthesi olon",
    "L�gg till alla",
    "", // TODO
    "", // TODO
    "Afegir-ho tot",
#if APIVERSNUM >= 10302
    "�������� ���",
#endif
  },
  { "Empty directory!",
    "Leeres Verzeichnis!",
    "Direktorij prazen!",
    "", // TODO
    "", // TODO
    "", // TODO
    "R�pertoire vide!",
    "", // TODO
    "Hakemisto on tyhj�!",
    "", // TODO
    "�Carpeta vac�a!", 
    "Adios fakelos!",
    "Tom katalog!",
    "", // TODO
    "", // TODO
    "Carpeta buida!",
#if APIVERSNUM >= 10302
    "������ �������!",
#endif
  },
  { "Add recursivly?",
    "Recursiv hinzuf�gen?",
    "Rekurzivno dodaj?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter r�cursivement?",
    "", // TODO
    "Lis�� rekursiivisesti?",
    "", // TODO
    "�A�adir recursivamente?", 
    "Prosthesi recursiv?",
    "L�gg till rekursivt?",
    "", // TODO
    "", // TODO
    "Afegir recursivament?",
#if APIVERSNUM >= 10302
    "�������� ����������?",
#endif
  },
  { "Remove entry?",
    "Eintrag entfernen?",
    "Odstrani izbrano?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever titre?",
    "", // TODO
    "Poistetaanko merkint�?",
    "", // TODO
    "�Borrar la entrada?", 
    "Aferesi simiou?",
    "Radera post?",
    "", // TODO
    "", // TODO
    "Esborrar la entrada?",
#if APIVERSNUM >= 10302
    "������� ������?",
#endif
  },
  { "Playlist editor",
    "Abspielliste bearbeiten",
    "Urejevalnik liste",
    "", // TODO
    "", // TODO
    "", // TODO
    "Editeur de liste de lecture",
    "", // TODO
    "Soittolistan muokkaus",
    "", // TODO
    "Editor de listas de reproducci�n", 
    "Metatropes stin Playlista",
    "Spellisteditor",
    "", // TODO
    "", // TODO
    "Editor de llistes de reproducci�",
#if APIVERSNUM >= 10302
    "�������� ������ �����",
#endif
  },
  { "Directory browser",
    "Verzeichnisanzeige",
    "Navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "Hakemistoselain",
    "", // TODO
    "Explorar carpetes", 
    "Endiski fakelou",
    "Katalogbl�ddrare",
    "", // TODO
    "", // TODO
    "Navegar per les carpetes",
#if APIVERSNUM >= 10302
    "�������� ���������",
#endif
  },
  { "Delete playlist?",
    "Abspielliste l�schen?",
    "Odstrani listo?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Effacer liste?",
    "", // TODO
    "Poistetaanko soittolista?",
    "", // TODO
    "�Borrar la lista?", 
    "Svisimo listas?",
    "Radera spellista?",
    "", // TODO
    "", // TODO
    "Esborrar la Llista?",
#if APIVERSNUM >= 10302
    "������� ������",
#endif
  },
  { "Are you sure?",
    "Wirklich l�schen?",
    "Ste prepricani?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Etes-vous s�r?",
    "", // TODO
    "Oletko varma?",
    "", // TODO
    "�Est� seguro?", 
    "Ise sigouros?",
    "�r du s�ker?",
    "", // TODO
    "", // TODO
    "N'esteu segur?",
#if APIVERSNUM >= 10302
    "�� �������?",
#endif
  },
  { "unnamed",
    "unbenannt",
    "neimenovan",
    "", // TODO
    "", // TODO
    "", // TODO
    "sansnom",
    "", // TODO
    "nimet�n",
    "", // TODO
    "sin nombre", 
    "xoris onoma",
    "namnl�s",
    "", // TODO
    "", // TODO
    "sense nom",
#if APIVERSNUM >= 10302
    "����������",
#endif
  },
  { "unknown",
    "unbekannt",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "inconnu",
    "", // TODO
    "tuntematon",
    "", // TODO
    "desconocido", 
    "", // TODO
    "ok�nd",
    "", // TODO
    "", // TODO
    "desconegut",
#if APIVERSNUM >= 10302
    "�����������",
#endif
  },
  { "Error scanning playlists!",
    "Fehler beim Einlesen der Abspiellisten!",
    "Napaka pri pregledovanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours de la liste!",
    "", // TODO
    "Soittolistojen lukeminen ep�onnistui!",
    "", // TODO
    "�Error al leer las listas!", 
    "Lathos stin sarosi tis Playlistas!",
    "Fel uppstod d� spellistorna l�stes!",
    "", // TODO
    "", // TODO
    "Error al llegir les llistes!",
#if APIVERSNUM >= 10302
    "������ ������ ������� �����!",
#endif
  },
  { "Error deleting playlist!",
    "Fehler beim L�schen der Abspielliste!",
    "Napaka pri odstranjevanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur d'effacement de la liste!",
    "", // TODO
    "Soittolistan poistaminen ep�onnistui!",
    "", // TODO
    "�Error al borrar una lista!",
    "Lathos stin akirosi tis Playlistas!",
    "Fel uppstod d� spellistan raderades!",
    "", // TODO
    "", // TODO
    "Error al esborrar una llista!",
#if APIVERSNUM >= 10302
    "������ �������� ������ �����!",
#endif
  },
  { "Error creating playlist!",
    "Fehler beim Erstellen der Abspielliste!",
    "Napaka pri kreiranju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de cr�ation de la liste!",
    "", // TODO
    "Soittolistan luominen ep�onnistui!",
    "", // TODO
    "�Error al crear una lista!",
    "Lathos stin dimiourgia tis Playlistas!",
    "Fel uppstod d� spellistan skapades!",
    "", // TODO
    "", // TODO
    "Error al crear una llista!",
#if APIVERSNUM >= 10302
    "������ �������� ������ �����!",
#endif
  },
  { "Can't edit a WinAmp playlist!",
    "Editieren von WinAmp Abspiellisten nicht m�glich!",
    "Ne morem vnesti liste od WinAmp-a!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ne peut pas �diter une liste WinAmp!",
    "", // TODO
    "WinAmp-soittolistaa ei voida muokata!",
    "", // TODO
    "�No puedo editar una lista del WinAmp!", 
    "Den mporo na kano metatropes se WinAmp Playlista!",
    "Kan inte redigera en WinAmp-spellista!",
    "", // TODO
    "", // TODO
    "No puc editar una llista del WinAmp!",
#if APIVERSNUM >= 10302
    "���������� ������������� ������ WinAmp!",
#endif
  },
  { "Error renaming playlist!",
    "Fehler beim Umbenennen der Abspielliste!",
    "Napaka pri preimenovanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur pour renommer la liste!",
    "", // TODO
    "Soittolistan uudelleennime�minen ep�onnistui!",
    "", // TODO
    "�Errir al renombrar la lista!", 
    "Latsos stin metonomasi tis Playlistas!",
    "Fel uppstod d� spellistan d�ptes om!",
    "", // TODO
    "", // TODO
    "Error al reanomenar la llista!",
#if APIVERSNUM >= 10302
    "������ �������������� ������!",
#endif
  },
  { "Loading playlist...",
    "Lade Abspielliste...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Chargement playlist...",
    "", // TODO
    "Avataan soittolistaa...",
    "", // TODO
    "Cargando la lista de reproducci�n...", 
    "", // TODO
    "L�ser in spellista...",
    "", // TODO
    "", // TODO
    "Carregant la llista de reproducci�...",
#if APIVERSNUM >= 10302
    "������ ������ �����...",
#endif
  },
  { "Error loading playlist!",
    "Fehler beim Lesen der Abspielliste!",
    "Napaka pri nalaganju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de chargement de la playlist!",
    "", // TODO
    "Soittolistan avaaminen ep�onnistui!",
    "", // TODO
    "�Error al cargar la lista de reproducci�n!", 
    "Lathos sto fortoma tis Playlistas!",
    "Fel uppstod d� spellistan l�stes in!",
    "", // TODO
    "", // TODO
    "Error al carregar la llista de reproducci�!",
#if APIVERSNUM >= 10302
    "������ ������ ������ �����!",
#endif
  },
  { "Scanning playlists...",
    "Durchsuche Abspiellisten...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Scanne playlists...",
    "", // TODO
    "Selataan soittolistoja...", 
    "", // TODO
    "Leyendo las listas de reproducci�n...",
    "", // TODO
    "S�ker spellistor...",
    "", // TODO
    "", // TODO
    "Llegint les llistes de reproducci�...",
#if APIVERSNUM >= 10302
    "����� ������� �����...",
#endif
  },
  { "Scanning directory...",
    "Durchsuche Verzeichnis...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Scanne le repertoire...",
    "", // TODO
    "Selataan hakemistoa...",
    "", // TODO
    "Leyendo las carpetas..",
    "", // TODO
    "S�ker igenom katalog...",
    "", // TODO
    "", // TODO
    "Revisant les carpetes..",
#if APIVERSNUM >= 10302
    "������������ ���������...",
#endif
  },
  { "Building playlist...",
    "Baue Abspielliste auf...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Construit la playlist...",
    "", // TODO
    "Muodostetaan soittolistaa...",
    "", // TODO
    "Construyendo la lista de reproducci�n...",
    "", // TODO
    "Skapar en spellista...",
    "", // TODO
    "", // TODO
    "Constru�nt la llista de reproducci�...",
#if APIVERSNUM >= 10302
    "���������� ������ �����...",
#endif
  },
  { "Error building playlist!",
    "Fehler beim Aufbau der Abspielliste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur construction playlist!",
    "", // TODO
    "Soittolistan muodostaminen ep�onnistui!",
    "", // TODO
    "�Error al construir la lista de reproducci�n!",
    "", // TODO
    "Fel uppstod d� spellistan skapades!",
    "", // TODO
    "", // TODO
    "Error al consstruir la llista de reproducci�!",
#if APIVERSNUM >= 10302
    "������ ���������� ������ �����!",
#endif
  },
  { "Remote CDDB lookup...",
    "Remote CDDB Abfrage...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Consultation CDDB...",
    "", // TODO
    "Kysyt��n CDDB-tietoja...",
    "", // TODO
    "Consultando CDDB...",
    "", // TODO
    "Fj�rrf�rfr�gan till CDDB...",
    "", // TODO
    "", // TODO
    "Consultant CDDB...",
#if APIVERSNUM >= 10302
    "������ � ��������� ���� CDDB...",
#endif
  },
  { "Connecting to stream server ...",
    "Verbinde mit Stream Server...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Connexion au Serveur Stream...",
    "", // TODO
    "Yhdistet��n palvelimeen...",
    "", // TODO
    "Conectando con el servidor...", 
    "", // TODO
    "Kontaktar stream-server...",
    "", // TODO
    "", // TODO
    "Connectant al servidor...",
#if APIVERSNUM >= 10302
    "����������� � ������� �������...",
#endif
  },
  { "Rename playlist",
    "Abspielliste umbenennen",
    "Preimenuj listo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer la liste",
    "", // TODO
    "Nime� soittolista",
    "", // TODO
    "Renombrar la lista de reproducci�n",
    "Metonomasi tis Playlistas",
    "D�p om spellistan",
    "", // TODO
    "", // TODO
    "Reanomenar la llista de repr.",
#if APIVERSNUM >= 10302
    "������������� ������ �����",
#endif
  },
  { "Old name:",
    "Alter Name:",
    "Staro ime:",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ancien nom:",
    "", // TODO
    "Vanha nimi",
    "", // TODO
    "Nombre anterior:", 
    "Palio onoma:",
    "Gammalt namn:",
    "", // TODO
    "", // TODO
    "Nom anterior:",
#if APIVERSNUM >= 10302
    "������ ���:",
#endif
  },
  { "New name",
    "Neuer Name",
    "Novo ime",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nouveau nom",
    "", // TODO
    "Uusi nimi",
    "", // TODO
    "Nombre nuevo", 
    "Neo onoma",
    "Nytt namn",
    "", // TODO
    "", // TODO
    "Nou nom", 
#if APIVERSNUM >= 10302
    "����� ���:",
#endif
  },
  { "Filenames",
    "Dateinamen",
    "Imena datotek",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noms de fichiers",
    "", // TODO
    "tiedostonimet",
    "", // TODO
    "Nombre de los archivos", 
    "Onomata arxeion",
    "Filnamn",
    "", // TODO
    "", 
    "Nom dels arxius",
#if APIVERSNUM >= 10302
    "����� ������",
#endif
  },
  { "ID3 names",
    "ID3 Namen",
    "ID3 imena",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom ID3",
    "", // TODO
    "ID3-nimet",
    "", // TODO
    "Nombre ID3", 
    "ID3 onomata",
    "ID3 namn",
    "", // TODO
    "", // TODO
    "Nom ID3",
#if APIVERSNUM >= 10302
    "ID3-�����",
#endif
  },
  { "ID3 information",
    "ID3 Information",
    "ID3 informacija",
    "", // TODO
    "", // TODO
    "", // TODO
    "Information ID3",
    "", // TODO
    "ID3-informaatio",
    "", // TODO
    "Informaci�n ID3",
    "Plirofories ID3",
    "ID3 information",
    "", // TODO
    "", // TODO
    "Informaci� ID3",
#if APIVERSNUM >= 10302
    "ID3-�����",
#endif
  },
  { "ID3 info",
    "ID3 Info",
    "ID3 info",
    "", // TODO
    "", // TODO
    "", // TODO
    "Info ID3",
    "", // TODO
    "ID3-info",
    "", // TODO
    "ID3 info",
    "ID3 pliroforia",
    "ID3 info",
    "", // TODO
    "", // TODO
    "ID3 info",
#if APIVERSNUM >= 10302
    "������ ID3",
#endif
  },
  { "Filename",
    "Dateiname",
    "Ime datoteke",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du fichier",
    "", // TODO
    "Tiedostonimi",
    "", // TODO
    "Nombre del archivo", 
    "Onoma arxeiou",
    "Filnamn",
    "", // TODO
    "", // TODO
    "Nom de l'arxiu",
#if APIVERSNUM >= 10302
    "��� �����",
#endif
  },
  { "Length",
    "L�nge",
    "Dolzina",
    "", // TODO
    "", // TODO
    "", // TODO
    "Longueur",
    "", // TODO
    "Pituus",
    "", // TODO
    "Duraci�n", 
    "Megethos",
    "L�ngd",
    "", // TODO
    "", // TODO
    "Durada",
#if APIVERSNUM >= 10302
    "�����",
#endif
  },
  { "Title",
    "Titel",
    "Naslov",
    "", // TODO
    "", // TODO
    "", // TODO
    "Titre",
    "", // TODO
    "Kappale",
    "", // TODO
    "T�tulo", 
    "Titlos",
    "Tittel",
    "", // TODO
    "", // TODO
    "T�tol",
#if APIVERSNUM >= 10302
    "��������",
#endif
  },
  { "Artist",
    "Interpret",
    "Avtor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Artiste",
    "", // TODO
    "Esitt�j�",
    "", // TODO
    "Artista",
    "Ermineftis",
    "Artist",
    "", // TODO
    "", // TODO
    "Int�rpret",
#if APIVERSNUM >= 10302
    "�����������",
#endif
  },
  { "Album",
    "Album",
    "Album",
    "", // TODO
    "", // TODO
    "", // TODO
    "Album",
    "", // TODO
    "Albumi",
    "", // TODO
    "Album",
    "Album",
    "Album",
    "", // TODO
    "", // TODO
    "�lbum",
#if APIVERSNUM >= 10302
    "������",
#endif
  },
  { "Year",
    "Jahr",
    "Leto",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ann�e",
    "", // TODO
    "Vuosi",
    "", // TODO
    "", // TODO
    "Etos",
    "�r",
    "", // TODO
    "", // TODO
    "Any",
#if APIVERSNUM >= 10302
    "���",
#endif
  },
  { "Samplerate",
    "Sample Rate",
    "Vzorcenje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Fr�quence",
    "", // TODO
    "N�ytteenottotaajuus",
    "", // TODO
    "Frecuencia", 
    "Sixnotita",
    "Samplingshastighet",
    "", // TODO
    "", // TODO
    "Freq��ncia",
#if APIVERSNUM >= 10302
    "�������",
#endif
  },
  { "Bitrate",
    "Bit Rate",
    "Bitna hitrost",
    "", // TODO
    "", // TODO
    "", // TODO
    "Bitrate",
    "", // TODO
    "Bittinopeus",
    "", // TODO
    "Bitrate",
    "Bitrate",
    "Bithastighet",
    "", // TODO
    "", // TODO
    "Bitrate",
#if APIVERSNUM >= 10302
    "�����",
#endif
  },
  { "Setup.MP3$Initial loop mode",
    "Default Loop Modus",
    "Osnovni nacin ponavljanje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de r�p�tition initial",
    "", // TODO
    "Jatkuva soitto oletuksena",
    "", // TODO
    "Modo de repetici�n inicial", 
    "", // TODO
    "Normalt upprepningsl�ge",
    "", // TODO
    "", // TODO
    "Mode de repetici� inicial",
#if APIVERSNUM >= 10302
    "����� ������� �� ���������",
#endif
  },
  { "Setup.MP3$Initial shuffle mode",
    "Default Shuffle Modus",
    "Osnovni nacin mesano",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de lecture al�atoire initial",
    "", // TODO
    "Satunnaissoitto oletuksena",
    "", // TODO
    "Lectura aleat�ria inicial", 
    "", // TODO
    "Normalt blandl�ge",
    "", // TODO
    "", // TODO
    "Lectura aleat�ria inicial",
#if APIVERSNUM >= 10302
    "����� �������. ������ �� ���������",
#endif
  },
  { "Setup.MP3$Title/Artist order",
    "Title/Artist Reihenfolge",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "N�yt� kappale ja esitt�j�",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Normal",
    "Normal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "normaalisti",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Reversed",
    "Umgedreht",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "k��nnettyn�",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Audio output mode",
    "Audio Ausgabe Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de sortie audio",
    "", // TODO
    "��nen ulostulomoodi",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "����� ������ �����",
#endif
  },
  { "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
#if APIVERSNUM >= 10302
    "OSS",
#endif
  },
  { "Setup.MP3$Audio mode",
    "Audio Modus",
    "Avdio nacin",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode audio",
    "", // TODO
    "��nimoodi",
    "", // TODO
    "Modo de audio", 
    "Katastasi audio",
    "Audiol�ge",
    "", // TODO
    "", // TODO
    "Mode d'audio",
#if APIVERSNUM >= 10302
    "����� ������������� �����",
#endif
  },
  { "Round",
    "Runden",
    "Zaokrozeni",
    "", // TODO
    "", // TODO
    "", // TODO
    "Arrondir",
    "", // TODO
    "py�ristetty",
    "", // TODO
    "Redondear", 
    "Kikli",
    "Avrunda",
    "", // TODO
    "", // TODO
    "Arrodonir",
#if APIVERSNUM >= 10302
    "�����������",
#endif
  },
  { "Dither",
    "Streuen",
    "Raztrosni",
    "", // TODO
    "", // TODO
    "", // TODO
    "Lisser",
    "", // TODO
    "ditteroitu",
    "", // TODO
    "Compactar",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Compactar",
#if APIVERSNUM >= 10302
    "�����������",
#endif
  },
  { "Setup.MP3$Background scan",
    "Hintergrund Scan",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Scan en tache de fond",
    "", // TODO
    "Taustaskannaus",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "������� ������������",
#endif
  },
  { "ID3 only",
    "nur ID3",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Seulement ID3",
    "", // TODO
    "vain ID3",
    "", // TODO
    "", 
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "������ ID3",
#endif
  },
  { "ID3 & Level",
    "ID3 & Pegel",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "ID3 et niveau",
    "", // TODO
    "ID3 & taso",
    "", // TODO
    "", 
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "ID3 � �������",
#endif
  },
  { "Setup.MP3$Editor display mode",
    "Editor Anzeige Modus",
    "Nacin prikaza urejevalnika",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage de l'�diteur",
    "", // TODO
    "Muokkaustilan n�ytt�moodi",
    "", // TODO
    "Modo de visualizaci�n del editor",
    "", // TODO
    "Redigerarens visuella l�ge",
    "", // TODO
    "", // TODO
    "Mode de visualitzaci� del editor",
#if APIVERSNUM >= 10302
    "����� ��������� �������",
#endif
  },
  { "Setup.MP3$Display mode",
    "Anzeige Modus",
    "Nacin prikaza",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage",
    "", // TODO
    "N�ytt�moodi",
    "", // TODO
    "Modo de Visualizaci�n", 
    "", // TODO
    "Visuellt l�ge",
    "", // TODO
    "", // TODO
    "Mode de visualitzaci�",
#if APIVERSNUM >= 10302
    "����� ����������",
#endif
  },
  { "Setup.MP3$Background mode",
    "Hintergrund Modus",
    "Ozadje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Arri�re plan",
    "", // TODO
    "Taustamoodi",
    "", // TODO
    "Ejecutar al fondo", 
    "", // TODO
    "Bakgrundsl�ge",
    "", // TODO
    "", // TODO
    "Execuci� de fons",
#if APIVERSNUM >= 10302
    "����� ����",
#endif
  },
  { "Setup.MP3$CDDB for CD-Audio",
    "CDDB f�r CD-Audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "CDDB pour CD-Audio",
    "", // TODO
    "CDDB-tietokanta",
    "", // TODO
    "CDDB para el Aidio-CD", 
    "", // TODO
    "CDDB f�r CD-audio",
    "", // TODO
    "", // TODO
    "CDDB per al Audio-CD",
#if APIVERSNUM >= 10302
    "CDDB ��� �����-CD",
#endif
  },
  { "disabled",
    "aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "d�sactiver",
    "", // TODO
    "pois",
    "", // TODO
    "desactivado", 
    "", // TODO
    "avst�ngd",
    "", // TODO
    "", // TODO
    "desactivat",
#if APIVERSNUM >= 10302
    "����",
#endif
  },
  { "local only",
    "nur lokal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local uniquement",
    "", // TODO
    "vain paikallinen",
    "", // TODO
    "solo local", 
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "solsament local",
#if APIVERSNUM >= 10302
    "������ �������.",
#endif
  },
  { "local&remote",
    "lokal und entfernt",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local et distant",
    "", // TODO
    "paikallinen & et�",
    "", // TODO
    "local y remoto",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local i remot",
#if APIVERSNUM >= 10302
    "�������. � ������.",
#endif
  },
  { "Setup.MP3$Use 48kHz mode only",
    "Nur 48kHz Modus benutzen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode 48kHz uniquement",
    "", // TODO
    "K�yt� vain 48kHz moodia",
    "", // TODO
    "Solo modo 48kHz",
    "", // TODO
    "Anv�nd endast 48Khz-l�ge",
    "", // TODO
    "", // TODO
    "Nom�s mode 48kHz",
#if APIVERSNUM >= 10302
    "������ 48 ڳ�",
#endif
  },
  { "Setup.MP3$Use HTTP proxy",
    "HTTP Proxy benutzen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Utiliser un Proxy HTTP",
    "", // TODO
    "K�yt� HTTP-v�lityspalvelinta",
    "", // TODO
    "Utilizar proxy HTTP", 
    "", // TODO
    "Anv�nd en HTTP-proxy",
    "", // TODO
    "", // TODO
    "Util�litzar proxy HTTP",
#if APIVERSNUM >= 10302
    "������������ HTTP ������",
#endif
  },
  { "Setup.MP3$HTTP proxy host",
    "HTTP Proxy Name",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du Proxy HTTP",
    "", // TODO
    "HTTP-v�lityspalvelimen osoite",
    "", // TODO
    "Nombre del puerto HTTP", 
    "", // TODO
    "HTTP-proxyns namn",
    "", // TODO
    "", // TODO
    "Nom del proxy HTTP",
#if APIVERSNUM >= 10302
    "������ HTTP ������",
#endif
  },
  { "Setup.MP3$HTTP proxy port",
    "HTTP Proxy Port",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Port du proxy HTTP",
    "", // TODO
    "HTTP-v�lityspalvelimen portti",
    "", // TODO
    "Puerto del Proxy HTTP", 
    "", // TODO
    "HTTP-proxyns port",
    "", // TODO
    "", // TODO
    "Port del proxy HTTP",
#if APIVERSNUM >= 10302
    "���� HTTP ������",
#endif
  },
  { "Setup.MP3$CDDB server",
    "CDDB Server",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Serveur CDDB",
    "", // TODO
    "CDDB-palvelin",
    "", // TODO
    "Servidor CDDB", 
    "", // TODO
    "CDDB server",
    "", // TODO
    "", // TODO
    "Servidor CDDB",
#if APIVERSNUM >= 10302
    "������ CDDB",
#endif
  },
  { "Setup.MP3$CDDB port",
    "CDDB Port",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Port CDDB",
    "", // TODO
    "CDDB-palvelimen portti",
    "", // TODO
    "Puerto CDDB",
    "", // TODO
    "CDDB port",
    "", // TODO
    "", // TODO
    "Port CDDB",
#if APIVERSNUM >= 10302
    "���� CDDB",
#endif
  },
  { "Black",
    "Schwarz",
    "Crnina",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noir",
    "", // TODO
    "musta",
    "", // TODO
    "Negra", 
    "Mavro",
    "Svart",
    "", // TODO
    "", // TODO
    "Negre",
#if APIVERSNUM >= 10302
    "������ �����",
#endif
  },
  { "Live",
    "Live",
    "Slika",
    "", // TODO
    "", // TODO
    "", // TODO
    "Live",
    "", // TODO
    "live",
    "", // TODO
    "En vivo", 
    "Zontana",
    "Live",
    "", // TODO
    "", // TODO
    "Imatge",
#if APIVERSNUM >= 10302
    "�����������",
#endif
  },
  { "Images",
    "Bilder",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Images",
    "", // TODO
    "kansikuva",
    "", // TODO
    "", 
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Mainmenu mode",
    "Hauptmenu Modus",
    "Nacin glavnega menija",
    "", // TODO
    "", // TODO
    "", // TODO
    "Menu principal",
    "", // TODO
    "P��valikon tila",
    "", // TODO
    "Men� Principal", 
    "", // TODO
    "Huvudmenyl�ge",
    "", // TODO
    "", // TODO
    "Men� Principal",
#if APIVERSNUM >= 10302
    "����� �������� ����",
#endif
  },
  { "Playlists",
    "Abspiellisten",
    "Lista",
    "", // TODO
    "", // TODO
    "", // TODO
    "Playlists",
    "", // TODO
    "soittolistat",
    "", // TODO
    "Lista de reproducci�n", 
    "Listes peksimatos",
    "Spellistor",
    "", // TODO
    "", // TODO
    "Llista a reprodu�r",
#if APIVERSNUM >= 10302
    "������ �����",
#endif
  },
  { "Browser",
    "Verz.anzeige",
    "Navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "selain",
    "", // TODO
    "Navegador",
    "", // TODO
    "Bl�ddra",
    "", // TODO
    "", // TODO
    "Navegador",
#if APIVERSNUM >= 10302
    "��������",
#endif
  },
  { "Setup.MP3$Normalizer level",
    "Normalisierer Pegel",
    "Normalni nivo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Niveau de normalisation",
    "", // TODO
    "Normalisoinnin taso",
    "", // TODO
    "Nivel de normalizaci�n", 
    "", // TODO
    "Normaliseringsniv�",
    "", // TODO
    "", // TODO
    "Nivell de normalitzaci�",
#if APIVERSNUM >= 10302
    "������� ������������",
#endif
  },
  { "Setup.MP3$Limiter level",
    "Begrenzer Pegel",
    "Limitni nivo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Niveau limite",
    "", // TODO
    "Rajoittimen taso",
    "", // TODO
    "Limitador del nivel", 
    "", // TODO
    "Begr�nsningsniv�",
    "", // TODO
    "", // TODO
    "Limitador del nivell",
#if APIVERSNUM >= 10302
    "����� ������������ �����",
#endif
  },
  { "Setup.MP3$Abort player at end of list",
    "Abspieler am Listenende beenden",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Stopper le lecteur � la fin de la liste",
    "", // TODO
    "Keskeyt� soittolistan loputtua",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "����� � ����� ������ �����",
#endif
  },
  { "Setup.MP3$Replay display",
    "Wiedergabe Anzeige",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Affichage relecture",
    "", // TODO
    "Toistotilan n�ytt�moodi",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "��������� � ������ ���������������",
#endif
  },
  { "classic",
    "klassisch",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Classique",
    "", // TODO
    "klassinen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "������������",
#endif
  },
  { "via skin",
    "�ber Skin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Par skin",
    "", // TODO
    "ulkoasun mukaan",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "�������� ����",
#endif
  },
  { "Setup.MP3$Keep selection menu",
    "Auswahlmenu ge�ffnet lassen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Pid� valintaikkunaa auki",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },

// start of MPlayer specific phrases

  { "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer-mediasoitin",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
#if APIVERSNUM >= 10302
    "������������� �����",
#endif
  },
  { "Media replay via MPlayer",
    "Medien Wiedergabe �ber MPlayer",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Lecture par MPlayer",
    "", // TODO
    "MPlayeriin perustuva mediasoitin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "������������� �����������",
#endif
  },
  { "MPlayer browser",
    "MPlayer Verzeichnisanzeige",
    "MPlayer navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur MPlayer",
    "", // TODO
    "MPlayer-hakemistoselain",
    "", // TODO
    "Navegador del MPlayer",
    "Mplayer endiksi fakelon",
    "MPlayer-bl�ddrare",
    "", // TODO
    "", // TODO
    "Navegador del MPlayer",
#if APIVERSNUM >= 10302
    "�������� ��������� MPlayer",
#endif
  },
  { "MPlayer source",
    "MPlayer Datentr�ger",
    "MPlayer izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source MPlayer",
    "", // TODO
    "MPlayer-l�hde",
    "Or�gen del Mplayer", 
    "", // TODO
    "Pigi MPlayer",
    "MPlayer k�lla",
    "", // TODO
    "", // TODO
    "Or�gen del MPlayer",
#if APIVERSNUM >= 10302
    "�������� ����� MPlayer",
#endif
  },
  { "Setup.MPlayer$Control mode",
    "Kontroll Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de controle",
    "", // TODO
    "Komentotila",
    "", // TODO
    "Modo de control", 
    "", // TODO
    "Kontroll�ge",
    "", // TODO
    "", // TODO
    "Mode de control",
#if APIVERSNUM >= 10302
    "����� ����������",
#endif
  },
  { "Traditional",
    "Traditionell",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Traditionnel",
    "", // TODO
    "perinteinen",
    "", // TODO
    "Tradicional", 
    "", // TODO
    "Traditionell",
    "", // TODO
    "", // TODO
    "Tradicional",
#if APIVERSNUM >= 10302
    "�������",
#endif
  },
  { "Slave",
    "Slave",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Esclave",
    "", // TODO
    "orja",
    "", // TODO
    "Esclavo", 
    "", // TODO
    "Slav",
    "", // TODO
    "", // TODO
    "Esclau",
#if APIVERSNUM >= 10302
    "�����������",
#endif
  },
  { "Setup.MPlayer$OSD position",
    "OSD Position",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Kuvaruutun�yt�n sijainti",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MPlayer$Resume mode",
    "Modus f�r Wiederaufnahme",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Reprise",
    "", // TODO
    "Resume-toiminto",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "local first",
    "zuerst local",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Local en premier",
    "", // TODO
    "paikallinen",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "global only",
    "nur global",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Global seulement",
    "", // TODO
    "globaali",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MPlayer$Slave command key",
    "Slave Kommando Taste",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Orjakomento n�pp�imelle",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "MPlayer Audio ID",
    "MPlayer Audio ID",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mplayer piste audio",
    "", // TODO
    "MPlayerin ��niraidan valinta",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Audiostream ID",
    "Tonspur ID",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Canal audio",
    "", // TODO
    "��niraidan ID",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },

  { NULL }
  };
