import os
import socket
from flask import Flask, jsonify, request
import pymysql

app = Flask(__name__)

DB_HOST = os.environ.get('DB_HOST', '127.0.0.1')
DB_PORT = int(os.environ.get('DB_PORT', '6033'))
DB_USER = os.environ.get('DB_USER', 'app')
DB_PASSWORD = os.environ.get('DB_PASSWORD', 'app_pass')
DB_NAME = os.environ.get('DB_NAME', 'highload_test')


def get_db_connection():
    return pymysql.connect(
        host=DB_HOST,
        port=DB_PORT,
        user=DB_USER,
        password=DB_PASSWORD,
        database=DB_NAME,
        connect_timeout=5
    )


@app.route('/')
def index():
    return jsonify({
        'app': 'highload-lab',
        'hostname': socket.gethostname(),
        'message': 'Hello from Flask cluster'
    })


@app.route('/health')
def health():
    try:
        conn = get_db_connection()
        with conn.cursor() as cur:
            cur.execute('SELECT 1')
            cur.fetchone()
        conn.close()
        return jsonify({'status': 'ok', 'hostname': socket.gethostname()}), 200
    except Exception as e:
        return jsonify({'status': 'error', 'error': str(e)}), 503


@app.route('/db/items', methods=['GET'])
def list_items():
    try:
        conn = get_db_connection()
        with conn.cursor() as cur:
            cur.execute('SELECT id FROM t1 ORDER BY id')
            rows = cur.fetchall()
        conn.close()
        return jsonify({'items': [r[0] for r in rows], 'hostname': socket.gethostname()})
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/db/items', methods=['POST'])
def add_item():
    try:
        item_id = int(request.json.get('id'))
        conn = get_db_connection()
        with conn.cursor() as cur:
            cur.execute('INSERT INTO t1 (id) VALUES (%s)', (item_id,))
        conn.commit()
        conn.close()
        return jsonify({'created': item_id, 'hostname': socket.gethostname()}), 201
    except Exception as e:
        return jsonify({'error': str(e)}), 500


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
