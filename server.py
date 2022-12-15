import subprocess
import string
from flask import Flask, render_template
app = Flask(__name__)

@app.route('/test/<path:filepath>')
def benchmark(filepath):
    clean = string.ascii_letters + string.digits + "_-"
    if any(c not in clean for c in filepath):
        return "No hacking"

    filepath = f"/connect4/benchmark/{filepath}"
    result = subprocess.run(["./benchmark", "<", filepath], capture_output=True).stdout
    print(result)
    test_info, time_info, search_info, speed_info = result.decode("ascii").strip().split("\n")
    return render_template("benchmark.html", test_info=test_info, time_info=time_info, search_info=search_info, speed_info=speed_info)

@app.route('/')
def hello_world():
    return render_template("index.html")

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
