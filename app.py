import json
import csv

from flask import request
from flask import jsonify
from flask import Flask
from flask import session
from flask import render_template
#https://python-adv-web-apps.readthedocs.io/en/latest/flask.html

#https://www.emqx.com/en/blog/how-to-use-mqtt-in-flask
from flask_mqtt import Mqtt
from flask_pymongo import PyMongo
from pymongo import MongoClient

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Initialisation :  Mongo DataBase

# Connect to Cluster Mongo : attention aux permissions "network"/MONGO  !!!!!!!!!!!!!!!!
ADMIN=False # Faut etre ADMIN/mongo pour ecrire dans la base
#client = MongoClient("mongodb+srv://menez:i.....Q@cluster0.x0zyf.mongodb.net/?retryWrites=true&w=majority")
#client = MongoClient("mongodb+srv://logincfsujet:pwdcfsujet@cluster0.x0zyf.mongodb.net/?retryWrites=true&w=majority")

#client = MongoClient("mongodb+srv://visitor:doliprane@cluster0.x0zyf.mongodb.net/?retryWrites=true&w=majority")
#client = MongoClient("mongodb+srv://toto:toto@cluster0.ccs6ez9.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0")
client = MongoClient("mongodb+srv://toto:toto@cluster0.ccs6ez9.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0")
# Print message if connection successful
if client:
    print("Connection to the MongoDB database successful!")

#-----------------------------------------------------------------------------
# Looking for "WaterBnB" database in the cluster
#https://stackoverflow.com/questions/32438661/check-database-exists-in-mongodb-using-pymongo
dbname= 'WaterBnB'
dbnames = client.list_database_names()
if dbname in dbnames: 
    print(f"{dbname} is there!")
else:
    #print("YOU HAVE to CREATE the db !\n")
    print("Creating the database...")
    db = client[dbname]  # Créer la base de données
    print(f"Database '{dbname}' created!")

db = client.WaterBnB
#-----------------------------------------------------------------------------
# Looking for "piscine" collection in the WaterBnB database
# Collection for pools
pools_collection = db.piscine

#@app.route("/reserve_pool", methods=['POST'])
def reserve_pool(pool_id, user_id):
    # Check if the pool exists
    pool = pools_collection.find_one({'_id': pool_id})
    if not pool:
        # If the pool doesn't exist, create it and mark as occupied by the user
        pool_data = {
            '_id': pool_id,
            'occupied': True,
            'id_user': user_id
        }
        pools_collection.insert_one(pool_data)
        return jsonify({'message': 'Pool created and reserved by user', 'pool_id': pool_id}), 200
    else:
        if pool['occupied']:
            # Cas où la piscine est déjà occupée
            return jsonify({'error': 'Pool already occupied'}), 400
        else:
            # Cas où l'utilisateur réserve la piscine car elle n'est pas occupé
            pools_collection.update_one({'_id': pool_id}, {'$set': {'occupied': True, 'id_user': user_id}})
            return jsonify({'message': 'Pool reserved by user', 'pool_id': pool_id}), 200
#-----------------------------------------------------------------------------
# Looking for "users" collection in the WaterBnB database
collname= 'users'
collnames = db.list_collection_names()
if collname in collnames: 
    print(f"{collname} is there!")
else:
    #print(f"YOU HAVE to CREATE the {collname} collection !\n")
    print(f"Creating the {collname} collection...")
    userscollection = db[collname]  # Créer la collection
    print(f"Collection '{collname}' created!")
    
userscollection = db.users

#-----------------------------------------------------------------------------
# import authorized users .. if not already in ?
if ADMIN :
    userscollection.delete_many({})  # empty collection
    excel = csv.reader(open("usersM1_2024.csv")) # list of authorized users
    for l in excel : #import in mongodb
        ls = (l[0].split(';'))
        #print(ls)
        if userscollection.find_one({"name" : ls[0]}) ==  None :
            userscollection.insert_one({"name": ls[0], "num": ls[1]})
    

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Initialisation :  Flask service
app = Flask(__name__)

# Notion de session ! .. to share between routes !
# https://flask-session.readthedocs.io/en/latest/quickstart.html
# https://testdriven.io/blog/flask-sessions/
# https://www.fullstackpython.com/flask-globals-session-examples.html
# https://stackoverflow.com/questions/49664010/using-variables-across-flask-routes
app.secret_key = 'BAD_SECRET_KEY'
  
#-----------------------------------------------------------------------------
@app.route('/')
def hello_world():
    return render_template('index.html') #'Hello, World!'

#Test with =>  curl https://waterbnb*ù$f.onrender.com/

#-----------------------------------------------------------------------------
"""
#https://stackabuse.com/how-to-get-users-ip-address-using-flask/
@app.route("/ask_for_access", methods=["POST"])
def get_my_ip():
    ip_addr = request.remote_addr
    return jsonify({'ip asking ': ip_addr}), 200

# Test/Compare with  =>curl  https://httpbin.org/ip

#Proxies can make this a little tricky, make sure to check out ProxyFix
#(Flask docs) if you are using one.
#Take a look at request.environ in your particular environment :
@app.route("/ask_for_access", methods=["POST"])
def client():
    ip_addr = request.environ['REMOTE_ADDR']
    return '<h1> Your IP address is:' + ip_addr
"""

#https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/X-Forwarded-For
#If a request goes through multiple proxies, the IP addresses of each successive proxy is listed.
# voir aussi le parsing !

@app.route("/open", methods= ['GET', 'POST'])
# @app.route('/open') # ou en GET seulement
def openthedoor():
    idu = request.args.get('idu') # idu : clientid of the service
    idswp = request.args.get('idswp')  #idswp : id of the swimming pool
    
    # Vérifier si l'utilisateur est déclaré dans la base de données
    #if userscollection.find_one({"name": idu}) is None:
        #return jsonify({'error': 'Utilisateur non autorisé'}), 403
    
    # Vérifier si la piscine existe et n'est pas déjà occupée
    # Appeler la fonction reserve_pool avec les valeurs de pool_id et user_id
    reserve_pool(idswp, idu)
    
    # Si toutes les conditions sont remplies, ouvrir la porte
    session['idu'] = idu
    session['idswp'] = idswp
    print("\n Peer = {}".format(idu))

    # ip addresses of the machine asking for opening
    ip_addr = request.environ.get('HTTP_X_FORWARDED_FOR', request.remote_addr)

    if userscollection.find_one({"name" : idu}) !=  None:
        granted = "YES"
    else:
        granted = "NO"
    return  jsonify({'idu' : session['idu'], 'idswp' : session['idswp'], "granted" : granted}), 200

# Test with => curl -X POST https://waterbnbf.onrender.com/open?who=gillou
# Test with => curl https://waterbnbf.onrender.com/open?who=gillou

@app.route("/users")
def lists_users(): # Liste des utilisateurs déclarés
    """
    curl https://waterbnbf.onrender.com/users
    """
    todos = userscollection.find()
    return jsonify([todo['name'] for todo in todos])

@app.route('/publish', methods=['POST'])
def publish_message():
    """
    mosquitto_sub -h test.mosquitto.org -t gillou
    mosquitto_pub -h test.mosquitto.org -t gillou -m tutu
    curl -X POST -H Content-Type:application/json -d "{\"topic\":\"gillou\",\"msg\":\"hello\"}"  https://waterbnbf.onrender.com/publish
    """
    content_type = request.headers.get('Content-Type')
    print("\n Content type = {}".format(content_type))
    request_data = request.get_json()
    print("\n topic = {}".format(request_data['topic']))
    
    publish_result = mqtt_client.publish(request_data['topic'], request_data['msg'])
    return jsonify({'code': publish_result[0]})

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%        
# Initialisation MQTT
app.config['MQTT_BROKER_URL'] =  "test.mosquitto.org"
app.config['MQTT_BROKER_PORT'] = 1883
#app.config['MQTT_USERNAME'] = ''  # Set this item when you need to verify username and password
#app.config['MQTT_PASSWORD'] = ''  # Set this item when you need to verify username and password
#app.config['MQTT_KEEPALIVE'] = 5  # Set KeepAlive time in seconds
app.config['MQTT_TLS_ENABLED'] = False  # If your broker supports TLS, set it True

topicname = "uca/iot/piscine"
mqtt_client = Mqtt(app)

@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
   if rc == 0:
       print('Connected successfully')
       mqtt_client.subscribe(topicname) # subscribe topic
   else:
       print('Bad connection. Code:', rc)


@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, msg):
    global topicname
    
    data = dict(
        topic=msg.topic,
        payload=msg.payload.decode()
    )
    #    print('Received message on topic: {topic} with payload: {payload}'.format(**data))
    print("\n msg.topic = {}".format(msg.topic))
    print("\n topicname = {}".format(topicname))
    
    if (msg.topic == topicname) : # cf https://stackoverflow.com/questions/63580034/paho-updating-userdata-from-on-message-callback
        decoded_message =str(msg.payload.decode("utf-8"))
        #print("\ndecoded message received = {}".format(decoded_message))
        dic =json.loads(decoded_message) # from string to dict
        print("\n Dictionnary  received = {}".format(dic))

        who = dic["info"]["ident"] # Qui a publié ?
        t = dic["status"]["temperature"] # Quelle température ?


#%%%%%%%%%%%%%  main driver function
if __name__ == '__main__':
    
    # run() method of Flask class runs the application 
    # on the local development server.
    app.run(debug=False) #host='127.0.0.1', port=5000)
    
