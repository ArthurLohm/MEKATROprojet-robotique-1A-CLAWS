#coding:utf-8
"""
Fichier realisant l'interface graphique permettant de commander le Robot CLAWS
Projet robotique ENS MEKATRO 1A (2026)

Architecture :

l'interface graphique est réalisé avec la bibliothèque customTKinter.
Le code élabore ensuite les commandes à envoyer à la carte Arduino à l'aide des options trouvé dans le fichier config.json
Il fait alors les requêtes au code C++ de l'auiduino qui reconstruit les commande de pilotage des servos à 
partir des bibliothèques robotis


"""
# Appel des bibliothèques

import customtkinter as ctk # pour l'interface graphique
import json # pour la lecture du fichier de configuration
import datetime # Pour ajouter l'heure aux logs
from math import atan2


##### Lecture du fichier de configuration
NOM_CONFIG="Config.json"

# lecture du fichier de congig json et conversion en dictionnaire python
with open(NOM_CONFIG, 'r', encoding='utf-8') as fichier:
    # json.load() lit le fichier et le transforme en dictionnaire Python
    config = json.load(fichier)





# Configuration globale
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")
def CodeurPos(pos):
    #Encode la trajectoire pour envoyer sous forme de chaines de caractère à arduino
    return f"{pos[0]}:{pos[1]}:{pos[2]}:{pos[3]}:{pos[4]}\n"

def MGI(pos):
    #MGI du robot renvoie la position articulaire à partir de la position opérationnel pos
    x,y,z,theta,P = pos
    l4 = config["rob"]["dimension"]["l4"]
    x += config["rob"]["corrections"]["xCorr"]
    y += config["rob"]["corrections"]["yCorr"]
    z += config["rob"]["corrections"]["zCorr"]

    gamma = config["rob"]["Pince"][str(P)] # On récupère l'angle du moteur pour l'ouverture de la pince

    return [z+l4,atan2(y,x),(x**2+y**2)**(1/2),theta,gamma]#[q1,q2,q3,q4,P] P est le niveau d'ouverture de la pince


def Executeur(trajOpe):

    trajArti=[]
    #On utilise le MGI pour obtenir la trajectoire dans les coordonné articulaire
    ajouter_log("Convertion de la trajectoire en coordonnée articulaire")
    for pos in trajOpe:
        trajArti.append(MGI(pos))

    #On code les positions à envoyer selon le protocole établis
    ajouter_log("Encodage de la trajectoire")
    trajArtiCod=[]
    for pos in trajArti:
        trajArtiCod.append(CodeurPos(pos))

    #On envoie chacune des pos à l'arduino
    ajouter_log("Envoie de la trajectoire")
    for pos in trajArtiCod:
        #arduino.write(trame.encode('utf-8'))
        ajouter_log(f"pos envoyé :{pos}","COMMANDE")


def precision():
    #Récupération des paramètres de la commande
    Niv = varNiv.get()
    Forme = varForme.get()

    ajouter_log(f"Précision, {Forme}, Niveau {Niv}")
    #Récupération de la liste des position clef de la trajectoire.
    trajOpe = config["rob"]["Formes"][Forme][Niv]
    print(trajOpe)
    Executeur(trajOpe)



# Création de la fenêtre
app = ctk.CTk()
app.geometry(config["Interface"]["global"]["geo"])
app.title(config["Interface"]["global"]["nom"])
app.resizable(False, False)#Bloque le changement de taille de l'interface


# ---------------- Fabrication de la fenêtre ---------------------------------
#création des cadres

#On construit les 2 collones de l'interface
FrameGauche = ctk.CTkFrame(app)
FrameGauche.pack(pady=0, padx=2, fill="both", expand=True,side="left")

FrameDroite = ctk.CTkFrame(app)
FrameDroite.pack(pady=0, padx=2, fill="both", expand=True,side ="right")


#-----On ajoute les éléments de la colonne de gauche-----

#On ajoute un Titre
TitreGauche = ctk.CTkLabel(FrameGauche,text="Commandes disponibles",font=tuple(config["Interface"]["Font"]["Titre2"]))
TitreGauche.pack(pady=5)
#Ajout du bouton de connexion
CConn = ctk.CTkFrame(FrameGauche,fg_color="transparent")
CConn.pack(pady=0, padx=0, fill="x", expand=True,side="top")
#Ajout du bouton de connexion
BConnexion = ctk.CTkButton(CConn,text="Connexion",command=lambda:ajouter_log("Connexion..."))
BConnexion.pack(pady=10,side="left")
Infoconnexion = ctk.CTkLabel(CConn,text="Non connecté",font=tuple(config["Interface"]["Font"]["Texte"]))
Infoconnexion.pack(pady=5,padx = 3,side="left")

#Ajout du bouton d'arrêt d'urgence
BArret = ctk.CTkButton(CConn,text="Arrêt d'urgence",command=lambda:ajouter_log("STOP"))
BArret.pack(pady=10,side="right")
#Pour la commande de Combat de tours
CCombat = ctk.CTkFrame(FrameGauche)
CCombat.pack(pady=1, padx=0, fill="x", expand=True,side="bottom")
#Ajout du titre
CCombatTitre = ctk.CTkLabel(CCombat,text="Combat de tours",font=tuple(config["Interface"]["Font"]["Titre2"]))
CCombatTitre.pack(pady=5)


#Pour la commande de contruction de tours

CTour = ctk.CTkFrame(FrameGauche)
CTour.pack(pady=1, padx=0, fill="x", expand=True,side="bottom")
#Ajout du titre
CTourTitre = ctk.CTkLabel(CTour,text="Construction d'une tour",font=tuple(config["Interface"]["Font"]["Titre2"]))
CTourTitre.pack(pady=5)
BTour = ctk.CTkButton(CTour,text="Ajouter un bloc",command=lambda:print("bloc"))
BTour.pack(pady=10,side="bottom")

#Pour la commande de placements des formes

#Création du cadre
CFormes = ctk.CTkFrame(FrameGauche)
CFormes.pack(pady=1, padx=0, fill="x", expand=True,side="bottom")
#Ajout du titre
CFormesTitre = ctk.CTkLabel(CFormes,text="Placement de formes",font=tuple(config["Interface"]["Font"]["Titre2"]))
CFormesTitre.pack(pady=5)

#Ajout du choix de la formes
CFrameChoix1 = ctk.CTkFrame(CFormes, fg_color="transparent")
CFrameChoix1.pack(pady=0, padx=0, fill="x", expand=True,side="top")
CFormesChoix1 = ctk.CTkLabel(CFrameChoix1,text="Formes à insérer :",font=tuple(config["Interface"]["Font"]["Texte"]))
CFormesChoix1.pack(pady=5,side ="left")

varForme =ctk.StringVar(value="Rond")
Rrond = ctk.CTkRadioButton(CFrameChoix1, text="Rond",variable=varForme, value="Rond")
Rrond.pack(pady=5,side="right")
Rcarre = ctk.CTkRadioButton(CFrameChoix1, text="Carré",variable=varForme, value="Carre")
Rcarre.pack(pady=5,side= "right")
Retoile = ctk.CTkRadioButton(CFrameChoix1, text="Etoile",variable=varForme, value="Etoile")
Retoile.pack(pady=5,side="right")

#Ajout du choix du niveau
CFrameChoix2 = ctk.CTkFrame(CFormes, fg_color="transparent")
CFrameChoix2.pack(pady=0, padx=0, fill="x", expand=True,side="top")
CFormesChoix2 = ctk.CTkLabel(CFrameChoix2,text="Niveau :",font=tuple(config["Interface"]["Font"]["Texte"]))
CFormesChoix2.pack(pady=5,side ="left")

varNiv =ctk.StringVar(value="1")
R3 = ctk.CTkRadioButton(CFrameChoix2, text="Niveau 3",variable=varNiv, value="3")
R3.pack(pady=5,side="right")
R2 = ctk.CTkRadioButton(CFrameChoix2, text="Niveau 2",variable=varNiv, value="2")
R2.pack(pady=5,side= "right")
R1 = ctk.CTkRadioButton(CFrameChoix2, text="Niveau 1",variable=varNiv, value="1")
R1.pack(pady=5,side="right")
# Ajout su boutton de lancement
BForme = ctk.CTkButton(CFormes,text="Lancer",command=precision)
BForme.pack(pady=10,side="bottom")




#-------Ajout des éléments dans la colonne de droite--------
#Ajout du titre
InfoTitre = ctk.CTkLabel(FrameDroite,text="Informations",font=tuple(config["Interface"]["Font"]["Titre1"]))
InfoTitre.pack(pady=0)
#Ajout du cadre des informations
CInfo = ctk.CTkFrame(FrameDroite)
CInfo.pack(pady=0, padx=0, fill="both", expand=True,side="top")


#Ajout du cadre des logs
LogsTitre = ctk.CTkLabel(FrameDroite,text="Logs",font=tuple(config["Interface"]["Font"]["Titre1"]))
LogsTitre.pack(pady=0,side="top")
Clogs = ctk.CTkFrame(FrameDroite)
Clogs.pack(pady=0, padx=0, fill="both", expand=True,side="bottom")
#on ajoute la zone pour les logs
zone_log = ctk.CTkTextbox(Clogs, width=400,height=300,wrap="word",font=tuple(config["Interface"]["Font"]["Console"]))
zone_log.pack(pady=0,padx=0)
zone_log.configure(state="disabled")#Par défaut on empêche l'écriture dans la zone des logs

def ajouter_log(message, type_log="INFO"):
    #On débloque la zone pour pouvoir y écrire
    zone_log.configure(state="normal")
    #Formatage du message avec l'heure
    heure = datetime.datetime.now().strftime("%H:%M:%S")
    ligne = f"[{heure}] {type_log} : {message}\n"
    #Insertion du texte à la toute fin ("end")
    zone_log.insert("end", ligne)
    #On force la barre de défilement à descendre tout en bas
    zone_log.see("end")
    #On rebloque la zone
    zone_log.configure(state="disabled")


#Ajout des fonctions appelé par les différents bouttons








# Lancement de l'application
app.mainloop()