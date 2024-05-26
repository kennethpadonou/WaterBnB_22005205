import datetime
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

from jsonschema import validate, ValidationError

# python 3.11

import random
import time

from paho.mqtt import client as mqtt_client


broker = 'test.mosquitto.org'
port = 1883
topic = "uca/iot/piscine/P_22005205"
# Generate a Client ID with the publish prefix.
client_id = f'publish-{random.randint(0, 1000)}'
# username = 'emqx'
# password = 'public'

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def publish(client):
    msg_count = 1
    while True:
        time.sleep(1)
        msg = f"messages: {msg_count}"
        result = client.publish(topic, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Send `{msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")
        msg_count += 1
        if msg_count > 5:
            break


def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)
    client.loop_stop()







#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Initialisation :  Mongo DataBase

# Connect to Cluster Mongo : attention aux permissions "network"/MONGO  !!!!!!!!!!!!!!!!
ADMIN=True # ADMIN=False Faut etre ADMIN/mongo pour ecrire dans la base
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


# Collection for pool state
pool_state_collection = db.pool_state

def save_pool_state(pool_id, user_id, user_name, state):
    current_time = datetime.datetime.now()
    state_data = {
        'pool_id': pool_id,
        'user_id': user_id,
        'user_name': user_name,
        'state': state,
        'datetime': current_time
    }
    pool_state_collection.insert_one(state_data)
    
    
def update_pool_state(pool_id, user_id, is_occupied): #def update_pool_state(pool_id, user_id, user_name, is_occupied):
    current_time = datetime.datetime.now()
    pool_state_data = {
        'pool_id': pool_id,
        'user_id': user_id,
        #'user_name': user_name,
        'is_occupied': is_occupied,
        'datetime': current_time
    }
    db.pool_state.insert_one(pool_state_data)
    print("Pool state updated and saved in the pool_state collection.")

#-----------------------------------------------------------------------------
# Looking for "piscine" collection in the WaterBnB database
collPool= 'piscine'
collnames = db.list_collection_names()
if collPool in collnames: 
    print(f"{collPool} is there!")
# Collection for pools
pools_collection = db.piscine

#@app.route("/reserve_pool", methods=['POST'])
def reserve_pool(pool_id, user_id):
    #publish_to_pool_topic("uca/iot/piscine/P_22005205", "Hello from Flask")
    
    
    # Check if the pool exists
    pool = pools_collection.find_one({'_id': pool_id})
    current_time = datetime.datetime.now()
    current_timeString = datetime.datetime.now().isoformat()
    if not pool:
        # If the pool doesn't exist, create it and mark as occupied by the user
        print(f"Pool {pool_id} does not exist. Creating a new pool and reserving it.")
        pool_data = {
            '_id': pool_id,
            'occupied': True,
            'id_user': user_id,
            'time': current_time
        }
        pools_collection.insert_one(pool_data)
        print(f"Pool {pool_id} created and reserved by user {user_id}.")
        is_occupied = True
        led_strip = "Jaune"
        message = {'occupied': is_occupied, 'led_strip': led_strip, 'time': current_time}
        #return jsonify({'message': 'Pool created and reserved by user', 'pool_id': pool_id, 'time': current_time}), 200
    else:
        print(f"Pool {pool_id} found: {pool}")
        if pool['occupied'] == True:
            # Case where the pool is already occupied
            print(f"Pool {pool_id} is already occupied.")
            is_occupied = True
            led_strip = "Rouge"
            #return jsonify({'error': 'Pool already occupied'}), 400
        else:
            # Case where the user reserves the pool as it is not occupied
            print(f"Pool {pool_id} is not occupied. Reserving the pool for user {user_id}.")
            print(f"Pool {pool_id} state before update: occupied = {pool['occupied']}")
            pools_collection.update_one({'_id': pool_id}, {'$set': {'occupied': True, 'id_user': user_id, 'time': current_time}})
            updated_pool = pools_collection.find_one({'_id': pool_id})
            print(f"Pool {pool_id} state after update: occupied = {updated_pool['occupied']}")
            is_occupied = False
            led_strip = "Jaune"
            #return jsonify({'message': 'Pool reserved by user', 'pool_id': pool_id, 'time': current_time}), 200

        message = {'occupied': is_occupied, 'led_strip': led_strip, 'time': current_timeString}
        
    update_pool_state(pool_id, user_id, is_occupied)
    # Publish the message to the user's topic 
    # Envoi du message MQTT
    publish_to_pool_topic("uca/iot/piscine/P_22005205", is_occupied,led_strip)
    publish_to_pool_topic("uca/iot/piscine/P_22005205", is_occupied,led_strip)
    publish_to_pool_topic("uca/iot/piscine/P_22005205", is_occupied,led_strip)
    print(f"Published message to topic {topic}: {message}")
    
    return jsonify({'message': 'okkk'}), 200







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
    count_before_insert = userscollection.estimated_document_count()
    for l in excel : #import in mongodb
        ls = (l[0].split(';'))
        #print(ls)
        if userscollection.find_one({"name" : ls[0]}) ==  None :
            userscollection.insert_one({"name": ls[0], "num": ls[1]})
    count_after_insert = userscollection.estimated_document_count()
    print(f"Total number of records before insertion: {count_before_insert}")
    print(f"Total number of records after insertion: {count_after_insert}")

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
@mqtt_client.on_connect()
def openthedoor():
    idu = request.args.get('idu') # idu : clientid of the service
    idswp = request.args.get('idswp')  #idswp : id of the swimming pool
    
    # Si toutes les conditions sont remplies, ouvrir la porte
    session['idu'] = idu
    session['idswp'] = idswp
    print("\n Peer = {}".format(idu))

    # ip addresses of the machine asking for opening
    ip_addr = request.environ.get('HTTP_X_FORWARDED_FOR', request.remote_addr)

    if userscollection.find_one({"num" : idu}) !=  None:
        granted = "YES"
        reserve_pool(idswp, idu)
        #reserve_response, status_code = reserve_pool(idswp, idu)
        #reserve_response, status_code = reserve_pool(idswp, idu)
        #reserve_data = reserve_response[0].get_json()
        #status_code = reserve_response[1]
    else:
        granted = "NO"
        #reserve_response, status_code = {}, 200
        #reserve_data = {}
        #status_code = 200
        
    response = {
        'idu': session['idu'],
        'idswp': session['idswp'],
        'granted': granted,
        #'reserve_response': reserve_data
    }
    return  jsonify(response), 200
    #return  jsonify({'idu' : session['idu'], 'idswp' : session['idswp'], "granted" : granted}), 200

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

def load_json_schema(schema_file):
    with open(schema_file, 'r') as file:
        schema = json.load(file)
    return schema
schema = load_json_schema('schema.json')

@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, msg):
    global topicname
    data = dict(
        topic=msg.topic,
        payload=msg.payload.decode()
    )
    print("\n msg.topic = {}".format(msg.topic))
    print("\n topicname = {}".format(topicname))
    
    if msg.topic == topicname:
        try:
            decoded_message = str(msg.payload.decode("utf-8"))
            if not decoded_message:
                print("Received an empty message, skipping JSON decoding.")
                return

            try:
                dic = json.loads(decoded_message)
            except json.JSONDecodeError as e:
                print(f"Failed to decode JSON: {e}")
                return

            try:
                validate(instance=dic, schema=schema)
                print("JSON correct")
            except ValidationError as e:
                print(f"Invalid JSON data: {e}")
                return
            
            #print("\n Dictionary received = {}".format(dic))

            who = dic["info"]["ident"]
            user_id = who[2:]  # Prend la sous-chaîne de who à partir du troisième caractère jusqu'à la fin
            pool = pools_collection.find_one({'_id': who})
            if pool:
                print(f"Current state of pool {who} in database: occupied = {pool.get('occupied', 'Not set')}")
                
                light_status = dic["status"]["light"]
                if light_status > 300 and pool.get('occupied', False):
                    pools_collection.update_one({'_id': who}, {'$set': {'occupied': False}})
                    update_pool_state(who, user_id, False)
                    print(f"Updated pool {who}: set occupied to False due to light level {light_status}")

                    # Envoi du message MQTT
                    topic = "uca/iot/piscine/" + who
                    current_time = datetime.datetime.now()
                    message = {
                        'occupied': False,
                        'led_strip': "vert",
                        'time': current_time.isoformat()
                    }
                    mqtt_client.publish(topic, json.dumps(message))
                    print(f"Published message to topic {topic}: {message}")
                else:
                    print(f"Pool {who} is not occupied or light level is below threshold, light = {light_status}")
            else:
                print(f"Pool {who} does not exist. Creating a new pool.")
                pool_data = {
                    '_id': who,
                    'occupied': False,
                    'id_user': user_id,
                    'time': datetime.datetime.now()
                }
                pools_collection.insert_one(pool_data)
                print(f"Pool {who} created.")
                pool = pools_collection.find_one({'_id': who})
        except Exception as e:
          print("Erreur au niveau de : " + str(e))

def publish_to_pool_topic(topic,occupied,color):
    app = Flask(__name__)
    app.secret_key = 'BAD_SECRET_KEY'
    app.config['MQTT_BROKER_URL'] = "test.mosquitto.org"
    app.config['MQTT_BROKER_PORT'] = 1883
    app.config['MQTT_TLS_ENABLED'] = False
    topicname = "uca/iot/piscine"
    my_ident = "P_22005205"
    mqtt_client = Mqtt(app)
    qos = 0
    update_color_topic = f"{topicname}/{my_ident}"
    result = mqtt_client.publish(update_color_topic, json.dumps({"occupied": occupied, "led_strip": color}).encode("utf-8"),
                            qos=qos)
    status = result[0]
    if status == 0:
        print(f"Send `{message}` to topic `{topic}`")
    else:
        print(f"Failed to send message to topic {topic}")
    print(f"Published message to topic {topic}: {message}")

#%%%%%%%%%%%%%  main driver function
if __name__ == '__main__':
    app.run(debug=False) 
    run()
    
