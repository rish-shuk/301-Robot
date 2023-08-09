%% This is a shell that you will have to follow strictly. 
% You will use the plotmap() and viewmap() to display the outcome of your algorithm.

% Load sample_data_map_8, three variables will be created in your workspace. These were created as a 
% result of [m,v,s]=dfs(15x19matrix,[14,1],[1,18]);
% The solution can be viewed using 
% plotmap(m,s) 

% write your own function for the AStar algorithm.
function [retmap,retvisited,retsteps] = astar_to_c( matrix,startlocation,targetlocation)
    map = matrix;
    [rows, cols] = size(map);   
    visitedMap = ones(rows, cols);
    
    % Define possible moves (up, down, left, right)
    moves = [0, 1; 0, -1; 1, 0; -1, 0];
    
    % Define function to check if a move is valid // MAKE SURE MAP(r,c) IS
    % 0 MEANING PATH
    isValidMove = @(r, c) r >= 1 && r <= rows && c >= 1 && c <= cols && map(r, c) == 0;
    emptyLocation = zeros(1,2);
    emptyPath = zeros(1,2);
    emptyF = zeros(1,1);
    % Initialize open list and closed list
    openList = struct('location', {emptyLocation}, 'path', {emptyPath}, 'f', {emptyF});
    openList(1).location = startlocation;
    openList(1).path = startlocation;
    openList(1).f = 0;
    closedList = [];
    
    steps = [];
    
    while ~isempty(openList)
        [~, minIndex] = min([openList.f]);
        current = openList(minIndex);
        openList(minIndex) = []; % Remove from open list
        
        row = current.location(1);
        col = current.location(2);
        
        if visitedMap(row, col) == 0
            continue;
        end
        
        visitedMap(row, col) = 0; % ADD VISITED TO CURRENT CELL
        
        %placestep([row, col], size(steps)); % PLACE STEP ON MAP --
        %irrelavnt

        if isequal([row, col], targetlocation)
            break;
        end
        
        for i = 1:size(moves, 1)
            newRow = row + moves(i, 1);
            newCol = col + moves(i, 2);
            
            if isValidMove(newRow, newCol) && visitedMap(newRow, newCol) == 1
                newPath = [current.path; newRow, newCol];
                steps = [steps; newRow, newCol];
                g = size(newPath, 1); % Distance from start
                h = abs(newRow - targetlocation(1)) + abs(newCol - targetlocation(2)); % Manhattan distance heuristic
                f = g + h;
                % G: Distance from the current location to the start location
                % H: distance from the current location to the target location
                % The lower the F cost, the more attractive it is as a path
                % options.
                % The goal is to choose the lowest F cost over and over
                % again until we reach the target location.
                newLocation = struct('location', [newRow, newCol], 'path', newPath, 'f', f);
                
                inOpenList = false;
                for j = 1:length(openList)
                    if isequal(openList(j).location, newLocation.location)
                        inOpenList = true;
                        break;
                    end
                end
                
                inClosedList = false;
                for j = 1:length(closedList)
                    if isequal(closedList(j).location, newLocation.location)
                        inClosedList = true;
                        break;
                    end
                end
                
                if ~inOpenList && ~inClosedList
                    openList(end+1) = newLocation;
                elseif inOpenList
                    existingIndex = find(ismember([openList.location], [newRow, newCol]));
                    existingF = openList(existingIndex).f;
                    if f < existingF
                        openList(existingIndex) = newLocation;
                    end
                end
            end
        end
        
        closedList = [closedList; current];
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
