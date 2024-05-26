# README

This is the [Flask](http://flask.pocoo.org/) [quick start](http://flask.pocoo.org/docs/1.0/quickstart/#a-minimal-application) example for [Render](https://render.com).

The app in this repo is deployed at [https://flask.onrender.com](https://flask.onrender.com).

## Deployment

Follow the guide at https://render.com/docs/deploy-flask.

## Lancement du projet

Installer les dependances via le fichier requierements.txt : pip install -r requirements.txt

 
## Lien important

- lien vers l'adresse [flask](https://waterbnb-22005205.onrender.com/)
-  lien pour réserver ma [piscine]([https://waterbnb-22005205.onrender.com/](https://waterbnb-22005205.onrender.com/open?idu=22005205&idswp=P_22005205))

- lien vers le [dashboard mongo](https://charts.mongodb.com/charts-angularmongo-yscamhi/public/dashboards/7b034796-d54d-4452-afad-c246efffab6e)

### Tips

Afin de s'assurer que le publish vers le topic de l'esp soit effectué, il faut parfois recharger 2 à 3 fois de suite la page /open.

#### Indications

- Pour réserver une piscine, placer votre doigt sur le capteur de lumière et cliquez sur réserver via la map du dashbord node-red !

  
.Les cas de figures pris en compte :
-  Si elle est verte alors la piscine est disponible à la location. C’est la couleur avant la demande
d’ouverture.
-  Si elle est jaune alors la piscine est désormais occupée (elle le reste tant que le capteur de lumière
le dit)
- Si elle est rouge alors l’accès est refusé. Elle reste dans cet état pendant 30 secondes puis revient
à vert.

 
