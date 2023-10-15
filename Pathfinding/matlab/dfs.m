%% This is a shell that you will have to follow strictly. 
% You will use the plotmap() and viewmap() to display the outcome of your algorithm.

% Load sample_data_map_8, three variables will be created in your workspace. These were created as a 
% result of [m,v,s]=dfs('map_8.txt',[14,1],[1,18]);
% The solution can be viewed using 
% plotmap(m,s) 

% write your own function for the DFS algorithm.
function [retmap,retvisited,retsteps] = dfs(mapfile,startlocation,targetlocation)
    map = map_convert(mapfile);
    [rows, cols] = size(map);
    visitedMap = ones(rows, cols);
    
    % Define possible moves (up, down, left, right)
    moves = [0, 1; 0, -1; 1, 0; -1, 0];
    
    % Define function to check if a move is valid // MAKE SURE MAP(r,c) IS
    % 0 MEANING PATH
    isValidMove = @(r, c) r >= 1 && r <= rows && c >= 1 && c <= cols && map(r, c) == 0;

    % Initialize stack for DFS
    stack = struct('location', startlocation, 'path', []);
    stack.path = [stack.path; startlocation];
    
    steps = []; % REMEMBER STEPS IS A VECTOR OF STEPS OF THE PATH TAKEN
    
    while ~isempty(stack)
        current = stack(end);
        stack(end) = []; % Pop from stack
        
        row = current.location(1);
        col = current.location(2);
        
        if visitedMap(row, col) == 0
            continue;
        end
        
        visitedMap(row, col) = 0; % ADD VISITED TO CURRENT CELL
        steps = [steps; row, col];   
        placestep([row, col], size(steps)); % PLACE STEP ON MAP

        if isequal([row, col], targetlocation)
            break;
        end
        
        for i = 1:size(moves, 1)
            newRow = row + moves(i, 1);
            newCol = col + moves(i, 2);
            
            if isValidMove(newRow, newCol) && visitedMap(newRow, newCol) == 1
                newStackItem = struct('location', [newRow, newCol], 'path', current.path);
                newStackItem.path = [newStackItem.path; newRow, newCol];
                stack = [stack; newStackItem];
            end
        end
    end
    
    retmap = map;
    retvisited = visitedMap;
    retsteps = steps;
end



function placestep(position,i)
% This function will plot a insert yellow rectangle and also print a number in this rectangle. Use with plotmap/viewmap. 
position = [16-position(1) position(2)];
position=[position(2)+0.1 position(1)+0.1];
rectangle('Position',[position,0.8,0.8],'FaceColor','y');
c=sprintf('%d',i);
text(position(1)+0.2,position(2)+0.2,c,'FontSize',10);
end
