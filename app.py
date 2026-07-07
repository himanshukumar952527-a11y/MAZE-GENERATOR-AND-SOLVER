from flask import Flask, render_template, request, jsonify
import subprocess, json, random

app = Flask(__name__)

# ===== PRIM (NATURAL LOOPS) =====
def generate_prim_natural(n, loop_prob=0.15):
    maze = [[1]*n for _ in range(n)]
    dirs = [(-2,0),(2,0),(0,-2),(0,2)]

    frontier = [(1,1)]
    maze[1][1] = 0

    while frontier:
        r,c = random.choice(frontier)
        frontier.remove((r,c))

        random.shuffle(dirs)

        for dr,dc in dirs:
            nr,nc = r+dr, c+dc

            if 1 <= nr < n-1 and 1 <= nc < n-1:

                # normal carve
                if maze[nr][nc] == 1:
                    maze[(r+nr)//2][(c+nc)//2] = 0
                    maze[nr][nc] = 0
                    frontier.append((nr,nc))

                # natural loop
                elif maze[nr][nc] == 0:
                    if maze[(r+nr)//2][(c+nc)//2] == 1:
                        if random.random() < loop_prob:
                            maze[(r+nr)//2][(c+nc)//2] = 0

    return maze


# Solver
def run_solver(grid):
    n = len(grid)

    with open("input.txt","w") as f:
        f.write(str(n)+"\n")
        for row in grid:
            f.write(" ".join(map(str,row))+"\n")
        f.write("1 1\n")
        f.write(f"{n-2} {n-2}\n")

    # subprocess.run(["gcc","maze_solver.c","-o","solver.exe"],check=True)
    # subprocess.run(["solver.exe"],check=True)
    subprocess.run(["gcc","maze_solver.c","-o","solver.exe"],check=True)
    subprocess.run(["solver.exe"],check=True)

    with open("output.json") as f:
        return json.load(f)


# ===== ROUTES =====
@app.route("/")
def home():
    return render_template("index.html")


@app.route("/generate-maze")
def generate():
    size = int(request.args.get("size",21))
    mode = request.args.get("mode","dfs")

    # ensure odd size
    if size % 2 == 0:
        size += 1

    #  MODE HANDLING 
    if mode == "prim":
        grid = generate_prim_natural(size)
        method = "PRIM (NATURAL LOOPS)"
        steps = None

    else:
        # DFS/BFS via C program
        method_flag = 1 if mode == "dfs" else 2

        # subprocess.run(["gcc","maze_making.c","-o","maker.exe"],check=True)
        # subprocess.run(["maker.exe",str(size),str(method_flag)],check=True)
        subprocess.run(["gcc","maze_making.c","-o","maker"],check=True)
        subprocess.run(["./maker",str(size),str(method_flag)],check=True)
        grid = []
        steps = None

        with open("input.txt") as f:
            n = int(f.readline())

            for _ in range(n):
                grid.append(list(map(int,f.readline().split())))

            # optional step count
            for line in f:
                if line.startswith("#steps"):
                    steps = int(line.split()[1])

        method = "DFS (C)" if method_flag == 1 else "BFS (C)"

    return jsonify({
        "grid": grid,
        "steps": steps,
        "method": method
    })


@app.route("/solve-maze",methods=["POST"])
def solve():
    grid = request.json["grid"]
    solution = run_solver(grid)
    return jsonify({"solution":solution})


if __name__=="__main__":
    app.run(debug=True)