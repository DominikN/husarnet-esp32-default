import sqlite3
import asyncio
import websockets
import argparse
import json
from sqlite3 import Error

conn = None
cur = None
sql_insert = "INSERT INTO exampledb (timestamp, output_type, value) VALUES (?, ?, ?);"

async def mainfunc():
    uri = "ws://" + args.hostname + ":8001"
    print("connecting: " + uri)
    async with websockets.connect(uri) as websocket:
        tx_msg = json.dumps({"set_output":args.outputType})

        print(tx_msg)
        await websocket.send(tx_msg)
        
        while True:
            rx_msg = await websocket.recv()
            print(rx_msg)
            rx_json = json.loads(rx_msg)

            cur.execute(sql_insert, (rx_json["timestamp"],rx_json["output_type"],rx_json["value"]))
            conn.commit()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("hostname", nargs='?', default="esp32basic")
    parser.add_argument("outputType", nargs='?', default="sine")
    args = parser.parse_args()

    print("hostname: ", args.hostname)
    print("outputType: ", args.outputType)

    try:
        conn = sqlite3.connect("./exampledb.db")
        print("sqlite version: " + sqlite3.version)
        cur = conn.cursor()
        cur.execute("CREATE TABLE IF NOT EXISTS exampledb (timestamp, output_type, value)")
        conn.commit()

        asyncio.get_event_loop().run_until_complete(mainfunc())

    except Error as e:
        print(e)
    except KeyboardInterrupt:
        print("\r\nReceived exit, exiting")
    finally:
        print("\r\nClose database connection")
        if conn:
            conn.close()