let mode = "dfs";

const canvas = document.getElementById("canvas");
const bfsCanvas = document.getElementById("bfsCanvas");
const dfsCanvas = document.getElementById("dfsCanvas");

const ctx = canvas.getContext("2d");
const bfsCtx = bfsCanvas.getContext("2d");
const dfsCtx = dfsCanvas.getContext("2d");

let grid = [], size = 21, cell = 20;

let genSteps = null;
let genMethod = "";

// ===== MODE SET =====
function setMode(m){
    mode = m;
    document.getElementById("status").innerText = `Mode: ${m.toUpperCase()}`;
}

// ===== DRAW BASE =====
function drawBase(ctx){
    ctx.fillStyle = "black";
    ctx.fillRect(0,0,canvas.width,canvas.height);

    for(let i=0;i<size;i++){
        for(let j=0;j<size;j++){
            if(grid[i][j] === 1){
                ctx.fillStyle = "#00ff88";
                ctx.fillRect(j*cell, i*cell, cell, cell);
            }
        }
    }
}

// ===== DRAW =====
function draw(){
    drawBase(ctx);
}

// ===== GENERATE =====
async function generateMaze(){

    size = parseInt(document.getElementById("gridSize").value);

    const res = await fetch(`/generate-maze?size=${size}&mode=${mode}`);
    const data = await res.json();

    grid = data.grid;
    genSteps = data.steps;
    genMethod = data.method;

    canvas.width = bfsCanvas.width = dfsCanvas.width = size * cell;
    canvas.height = bfsCanvas.height = dfsCanvas.height = size * cell;

    draw();

    bfsCtx.clearRect(0,0,canvas.width,canvas.height);
    dfsCtx.clearRect(0,0,canvas.width,canvas.height);

    document.getElementById("status").innerText =
        genSteps !== null
        ? `Generated using ${genMethod} | Steps: ${genSteps}`
        : `Generated using ${genMethod}`;
}

// ===== SOLVE =====
async function solveMaze(){

    if(grid.length === 0){
        alert("Generate maze first!");
        return;
    }

    const res = await fetch("/solve-maze",{
        method:"POST",
        headers:{"Content-Type":"application/json"},
        body:JSON.stringify({grid})
    });

    const data = await res.json();

    let bfsPath = data.solution.bfs.path;
    let dfsPath = data.solution.dfs.path;

    drawSolution(bfsCtx, bfsPath, "red");
    drawSolution(dfsCtx, dfsPath, "blue");

    // 🔥 if both same → find alternate
    if(isSamePath(bfsPath, dfsPath)){

        let altPath = findAlternateDFS(grid, dfsPath);

        if(altPath){
            drawSolution(dfsCtx, altPath, "yellow");
        }
    }

    let genText =
        genSteps !== null
        ? `Generated using ${genMethod} | Steps: ${genSteps}`
        : `Generated using ${genMethod}`;

    let solveText =
        `Solve → BFS Steps: ${data.solution.bfs.steps} | DFS Steps: ${data.solution.dfs.steps}`;

    document.getElementById("status").innerHTML =
        `${genText}<br>${solveText}`;
}

// ===== DRAW PATH =====
function drawSolution(ctx, path, color){

    drawBase(ctx);

    path.forEach(([r,c])=>{
        ctx.fillStyle = color;
        ctx.fillRect(c*cell, r*cell, cell, cell);
    });
}

// ===== SAME PATH CHECK (FIXED) =====
function isSamePath(p1, p2){
    if(p1.length !== p2.length) return false;

    for(let i=0;i<p1.length;i++){
        if(p1[i][0] !== p2[i][0] || p1[i][1] !== p2[i][1]){
            return false;
        }
    }
    return true;
}

// ===== 🔥 NATURAL ALTERNATE PATH FINDER =====
function findAlternateDFS(grid, originalPath){

    let n = grid.length;

    // try multiple times (important)
    for(let attempt = 0; attempt < 5; attempt++){

        let visited = Array.from({length:n},()=>Array(n).fill(false));
        let path = [];

        // 🔥 partial blocking (not full path)
        let blocked = new Set();

        for(let [r,c] of originalPath.slice(1,-1)){
            if(Math.random() < 0.4){
                blocked.add(r+","+c);
            }
        }

        function dfs(r,c){

            if(r === n-2 && c === n-2){
                path.push([r,c]);
                return true;
            }

            visited[r][c] = true;
            path.push([r,c]);

            let dirs = [[-1,0],[1,0],[0,-1],[0,1]];
            dirs.sort(()=>Math.random()-0.5);

            for(let [dr,dc] of dirs){
                let nr = r+dr, nc = c+dc;

                if(nr>=0 && nc>=0 && nr<n && nc<n &&
                   grid[nr][nc] === 0 &&
                   !visited[nr][nc] &&
                   !blocked.has(nr+","+nc))
                {
                    if(dfs(nr,nc)) return true;
                }
            }

            path.pop();
            return false;
        }

        if(dfs(1,1)) return path;
    }

    return null;
}