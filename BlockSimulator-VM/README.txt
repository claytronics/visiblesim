BlockSimulator v1.0


1 Introduction
--------------

BlocSimulator a pour objectif de simuler des SmartBlocs. Les SmartBlocks sont des dispositifs physiques de forme cubique. 
- Ils sont indépendants les uns des autres
- Ils évoluent sur une surface plane spéciale.
- Ils disposent de capteurs et de ports de communication sur leurs faces latérales pour détecter et communiquer avec les SmartBlocs voisins.
- Ils peuvent se déplacer en "glissant" contre leurs voisins.

- Ils ne disposent pas d'un système de positionnement global




2 Principes de fonctionnement du simulateur
-------------------------------------------

BlocSimulator simule l'environnement physique
- Un bloc ne peut pas en traverser un autre.
- Un bloc ne peut communiquer qu'avec les blocs avec lesquels il est en contact

BlocSimulator est un simulateur à événements discrets.
A ce titre, chaque action est modélisée sous la forme d'un événement qui possède une date de survenance.

Quelques exemples d'événements:
- Réception d'un message
- Fin d'une action de déplacement
- Entrée en contact de deux blocs

Les événements sont insérés dans une liste ordonnée, qui va être ensuite "éclusée" chronologiquement.
Bien sûr, au cours du traitement des événements, il est tout à fait possible de d'insérer de nouveaux événements dans la liste.

La simulation se termine lorsqu'il n'y a plus d'événements dans la liste ou qu'une certaine date maximale est atteinte (propriété Scheduler::heureMaximale)

Les dates sont gérées en micro-secondes (variable de type uint64_t)


3 Implémentation et détails du fonctionnement
---------------------------------------------

3.1 Principales classes
-----------------------

- Scheduler :
C'est le gestionnaire des événements. Il ne peut y en avoir qu'un seul dans le programme.
Ses propriétés et méthodes sont statiques.

- VisualiseurGraphique :
Cette classe se charge de l'affichage OpenGL
Il n'y en a qu'un seul dans le programme, ses propriétés et méthodes sont statiques
(Au début j'avais prévu de tout faire par l'intermédiaire d'une instance de cette classe, puis je suis passé à un usage statique ... 
dans beaucoup d'endroits c'est encore l'instance qui est utilisée)

- Bloc :
Modélise le SmartBlock physique. Quelqu'un souhaitant développer des algorithmes pour les SmartBlocks n'est pas sensé modifier cette classe.
Par contre il faut y accéder pour avoir accès aux références sur les voisins actuellements en contact (voisinNord, voisinsSud, etc.)

- CodeBloc :
Le programme qui tourne dans les SmartBlocks. Chaque algorithme devrait être implémenté dans une classe dérivée de CodeBloc
Chaque instance possède une référence sur le bloc éxécutant ce code (proriété "bloc").

- Evenement :
La classe décrivant un événement générique. Chaque type particulier d'événement doit être implémenté par une classe dérivée d'Evenement.

- Message :
La classe de base pour les messages. Les messages spécifiques à chaque algorithme doivent être modélisés par des classes dérivées de celle-ci.


3.2 Détail du fonctionnement des CodeBloc
-----------------------------------------

Chaque algorithme doit être implémenté dans une classe dérivée de CodeBloc.
Deux méthodes doivent absolument être implémentées :
demarrage() : qui est appelée automatiquement lorsque survient un événement EvenementDemarreCodeBloc
traiteEvenementLocal(Evenement *) : qui est appelée automatiquement pour traiter chaque événement dans la file locale du Bloc.

Chaque bloc possède une file locale des événements à traiter. ATTENTION, c'est une file FIFO.
Cette file sert à modéliser les temps de traitement.
Si plusieurs événéments arrivent en même temps ils seront traités séquentiellement grâce à cette file.
A cause de cette file, ils seront éventuellement traités à une date postérieure à la date normale de l'événement.
Un appel à traiteEvenementLocal(Evenement *) est réalisé pour chaque événement de la file.
Il appartient au concepteur de l'algorithme de tester le type de l'événement dans traiteEvenementLocal(Evenement *)
(voir l'exemple dans CodeBlocDemo2)

Pour modéliser le temps de traitement, au cours d'un appel à traiteEvenementLocal, il suffit de modifier la propriété heureDisponibilite de CodeBloc.
De cette manière, le prochain appel à la fonction se fera au plus tôt à l'heure indiquée (ou plus tard si il n'y a pas d'événements en attente dans la liste locale)

Il est possible de modéliser le fait qu'il peut falloir du temps entre le début de l'appel à traiteEvenementLocal et le moment où une décision d'action est prise.
(par exemple analyser des données locales pour déterminer si on veut bouger)
Pour celà, il suffit de programmer l'action (ex: bouger) à une date supérieure à la dateActuelle.

Les actions sont gérées de manière asynchrone. Cela signifie que l'on ne peut pas savoir immédiatement si des actions telles que se déplacer ou envoyer un message ont réussi.
Dans ces cas là, on programme un événement (par exemple EvenementDebutDeplaceBloc). Si au final le déplacement s'est bien déroulé, on va recevoir un événement EvenementFinDeplaceBloc.
Si le déplacement a échoué, on recevra un EvenementEchecDebutDeplaceBloc.

3.3 Détail du fonctionnement de la gestion des événements
---------------------------------------------------------

Le Scheduler ne sait manipuler que les objets de la classe Evenement
Les événements sont donc tous implémentés sous la forme de classes dérivées de la classe Evenement.

Ces sous-classes doivent implémenter une méthode "consomme()" qui est appelée par le Scheduler à la date de survenance de l'événement.

L'implémentation permet de tenir compte des temps de traitement des événemens par les CodeBloc grâce à la notion de disponibilité.
Si un événement doit être traité par un CodeBloc, il le sera lorsque le CodeBloc sera devenu disponible.

Un événement reçu à "t0" et qui demande un temps de traitement "d" sera traité en deux temps:
a) à t0, certaines actions immédiates
b) la programmation d'un nouvel événement qui appelera lui même la méthode traiteEvenementLocal du CodeBloc concerné.
(si le CodeBloc est disponible, l'appel à traiteEvenementLocal est immédiat, sinon le traitement est mis en attente)

Ainsi, bien que l'execution de la méthode traiteEvenementLocal soit instantanée du point de vue du Scheduler, 
la décision du CodeBloc sera effectivement prise à "t0+d" sur les données reçues à "t0" et avec le bloc et un environnement dans un état à "t0+d".
(par exemple, lors du traitement, les voisins visibles sont ceux visibles à t0+d, et pas ceux visibles à t0 !) 

 Pour celà, les actions se déroulent dans cet ordre:
a) le Scheduler prend le premier événement de la liste globale des événements
b) le Scheduler appelle la méthode "consomme()" de cet événement
c) la méthode consomme() réalise certains traitements "immédiats" (par exemple si l'événement indique la fin d'un déplacement, 
elle met à jour la positionDepart du bloc)
d) on met l'événement en attente dans la file locale du CodeBloc
e) si aucun autre élément n'est en attente mais que le CodeBloc n'est pas encore dispobible, 
	- on programme un événement EvenementCodeBlocTraiteEvenementLocal

f) lorsqu'un EvenementCodeBlocTraiteEvenementLocal survient, le premier événement de la liste locale est traité
g) si ce traitement prend du temps
    - l'heure à laquelle le CodeBloc sera disponible pour le prochain traitement d'événement est mise à jour
h) puis si il reste un événement dans la liste locale
    -reprogramme un EvenementCodeBlocTraiteEvenementLocal pour le moment ou le CodeBloc redeviendra disponible 

A noter que dans le code de décision, il appartient au programmeur de mettre à jour dateDisponibilite afin de modéliser le fait que les traitements prennent du temps.
Enfin, si l'on veut modéliser le fait que les actions décidées ne sont pas immédiates, il suffit de les programmer dans le futur (par exemple un déplacement ou l'envoi d'un message)

3.4 Libération de la mémoire occupée par les Evenement et les Message
---------------------------------------------------------------------

De manière à faciliter le travail d'un concepteur d'algorithme, la destruction de ces objets est automatique.

ATTENTION, pour des raisons de performances, il peut à un moment donné y avoir plusieurs références (pointeurs) vers un même objet. 
Le programme comptabilise ces références et va tâcher de ne détruire l'objet que si ce compteur arrive à 0.

Si vous avez besoin de conserver quelque part un Message ou un Evenement il est donc recommandé d'un faire une copie complète, sous peine de le voire sinon libéré intempestivement par le programme.
(le code de libération automatique se trouve dans les classes Scheduler et CodeBloc)

4 Au sujet des performances
---------------------------

Le nombre de blocs gérés est limité seulement par la mémoire disponible.
Suivant la nature des algorithmes testé, attention au nombre d'événements et de messages qui peuvent être mis en attente.
(Lorsque le programme se termine, il donne quelques valeurs indicatives sur la simulation qui vient de se dérouler)

La liste des événements est actuellement implémentée par une liste doublement chaînée (classe list de la STL).
Lorsque l'on programme un nouvel événement, le Scheduler va parcourir cette liste depuis la fin pour trouver le bon endroit auquel l'insérer.
Cela signifie que le temps d'insertion d'un élément dans la liste peut croitre rapidement avec la taille de la liste 
(surtout si on programme des événements pour un futur immédiat ou proche alors qu'il y a déjà beaucoup d'événements plus lointains).

Il est envisageable de remplacer cette liste doublement chaînée par une structure dans laquelle l'insertion coûte moins cher (par exemple un arbre binaire) ...
Mais en l'état, il est possible de traiter au moins plusieurs dizaines de milliers d'événements par seconde
