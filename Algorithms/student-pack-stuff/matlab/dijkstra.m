% write your own function for the dijkstra algorithm.
function [retmap, retvisited, retsteps] = dijkstra(mapfile, startlocation, targetlocation)
    map = map_convert(mapfile);
    [rows, cols] = size(map);
    visitedMap = ones(rows, cols); % records visited points

    % Define possible moves (up, down, left, right)
    moves = [0, 1; 0, -1; 1, 0; -1, 0];

    % Define function to check if a move is valid // MAKE SURE MAP(r,c) IS
    % 0 MEANING PATH
    isValidMove = @(r, c) r >= 1 && r <= rows && c >= 1 && c <= cols && map(r, c) == 0;

    % begin implementation of dijkstra's algorithm
    % create heap of locations and their distances
    pq = struct('location', startlocation, 'distance', 0);
    
    % Initialize distances and paths arrays
    distances = Inf(rows, cols);
    paths = cell(rows, cols);
    
    % Initialize start location's distance and path
    distances(startlocation(1), startlocation(2)) = 0;
    paths{startlocation(1), startlocation(2)} = startlocation;

    while ~isempty(pq)
        % Extract node with the smallest distance from priority queue
        [~, idx] = min([pq.distance]);
        current = pq(idx);
        pq(idx) = [];

        row = current.location(1);
        col = current.location(2);

        % Check if the node has been visited
        if visitedMap(row, col) == 1
            visitedMap(row, col) = 0;
            % no need to update steps and placestep since we're using distances and paths
            
            % If neighbor is target location, then finish
            if isequal([row, col], targetlocation)
                break;
            end

            % Explore neighbors and update distances and paths
            for i = 1:size(moves, 1)
                newRow = row + moves(i, 1);
                newCol = col + moves(i, 2);

                if isValidMove(newRow, newCol) && visitedMap(newRow, newCol) == 1
                    newDistance = distances(row, col) + 1;

                    % Update distance and path if shorter path found
                    if newDistance < distances(newRow, newCol)
                        distances(newRow, newCol) = newDistance;
                        paths{newRow, newCol} = [paths{row, col}; [newRow, newCol]];

                        % Add to priority queue
                        newPQItem = struct('location', [newRow, newCol], 'distance', newDistance);
                        pq = [pq, newPQItem];
                    end
                end
            end
        end
    end
    
    % Return the map, visited map, and the path
    retmap = map;
    retvisited = visitedMap;
    retsteps = paths{targetlocation(1), targetlocation(2)};

    % Call placestep function for each step in the path
    for i = 1:size(retsteps, 1)
        placestep([retsteps(i, 1), retsteps(i, 2)], i);
    end
end

function placestep(position,i)
% This function will plot a insert yellow rectangle and also print a number in this rectangle. Use with plotmap/viewmap. 
position = [16 - position(1), position(2)];
position = [position(2) + 0.1, position(1) + 0.1];
rectangle('Position', [position, 0.8, 0.8], 'FaceColor', 'y');
c = sprintf('%d', i);
text(position(1) + 0.2, position(2) + 0.2, c, 'FontSize', 10);
end
