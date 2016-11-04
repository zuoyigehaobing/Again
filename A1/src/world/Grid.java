package world;

import heap.Heap;
import heap.HeapEmptyException;
import heap.HeapFullException;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;
import java.util.Set;

public class Grid {

	private final int DEFAULT_WIDTH = 60; // default width of the world map - gridX runs from 0 to 59
	private final int DEFAULT_HEIGHT = 15; // default height of the map - gridY runs from 0 to 14
	private final int DEFAULT_PERCENT = 20; // this is the percentageof the map occupied by islands
	protected int width, height; // user defined width and height, if one is not using defaults
	protected int percent; // user defined percentage of islands on the map
	protected Node treasure; // points to the map node where the Redbeard treasure is sunken
	protected Node boat; // points to the current location of our boat on the map

	protected Node[][] map; // the map

	public Grid() {
		width = DEFAULT_WIDTH;
		height = DEFAULT_HEIGHT;
		percent = DEFAULT_PERCENT;
		buildMap();
	}

	public Grid(int width, int height, int percent) {
		this.width = width;
		this.height = height;
		if (percent <= 0 || percent >= 100)
			this.percent = DEFAULT_PERCENT;
		else
			this.percent = percent;
		buildMap();
	}

	private void buildMap() {
		// Your implementation goes here
		// For each map position (i,j) you need to generate a Node with can be navigable or it may belong to an island
		// You may use ideas from Lab3 here.
		// Don't forget to generate the location of the boat and of the treasure; they must be on navigable waters, not on the land!
		
		map = new Node[height][width];
		Random rand = new Random();
		// put the boat randomly
		int h,w;
		h = rand.nextInt(height);
		w = rand.nextInt(width);
		boat = new Node(true,w,h);
		map[h][w] = boat;
		
		// put the treasure randomly, make sure the position is not same as positon of the boat
		boolean put_treasure = false;
		
		while (put_treasure == false){
			h = rand.nextInt(height);
			w = rand.nextInt(width);
			if (map[h][w] == null) {
				treasure = new Node(true, w, h);
				map[h][w] = treasure;
				put_treasure = true;
			}
		}
		
		// fill other grid with island or walkable grid
		for(int i=0; i < height; i++)
			for(int j= 0 ; j <width; j++){
				if (map[i][j] == null){
					boolean obstacle = (rand.nextInt(99) < percent);
					// put island
					if (obstacle){
						map[i][j] = new Node(false,j,i);
					}
					// walkable
					else{
						map[i][j] = new Node(true,j,i);
					}
				}
			}
		
	}

	public String drawMap() {
		// provided for your convenience
		String result = "";
		String hline = "       ";
		String extraSpace;
		for (int i = 0; i < width / 10; i++)
			hline += "         " + (i + 1);
		result += hline + "\n";
		hline = "       ";
		for (int i = 0; i < width; i++)
			hline += (i % 10);
		result += hline + "\n";
		for (int i = 0; i < height; i++) {
			if (i < 10)
				extraSpace = "      ";
			else
				extraSpace = "     ";
			hline = extraSpace + i;
			for (int j = 0; j < width; j++) {
				if (i == boat.gridY && j == boat.gridX)
					hline += "B";
				else if (i == treasure.gridY && j == treasure.gridX)
					hline += "T";
				else if (map[i][j].inPath)
					hline += "*";
				else if (map[i][j].walkable)
					hline += ".";
				else
					hline += "+";
			}
			result += hline + i + "\n";
		}
		hline = "       ";
		for (int i = 0; i < width; i++)
			hline += (i % 10);
		result += hline + "\n";
		return result;
	}
	
	public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

	public int getPercent() {
		return percent;
	}
	
	public Node getBoat() {
		return boat;
	}
	
	private ArrayList<Node> getNeighbours(Node node) {
		// each node has at most 8 neighbours
		// Lab3 may be useful here as well
		ArrayList<Node> result = new ArrayList<Node>();
		boolean North,South,East,West;
		int h,w;
		h = node.gridY;
		w = node.gridX;
		North = (h - 1) >= 0;
		South = (h + 1) < height;
		East = (w + 1) < width;
		West = (w - 1) >= 0;
		
		// element in the north
		if (North) result.add(map[h-1][w]);
		
		// element in the north-east
		if (North && East) result.add(map[h-1][w+1]);
		
		// element in the east
		if (East) result.add(map[h][w+1]);
		
		// element in the south-east
		if (South && East) result.add(map[h+1][w+1]);
		
		// element in the south
		if (South) result.add(map[h+1][w]);
		
		// element in the south-west
		if (South && West) result.add(map[h+1][w-1]);
		
		// element in the west
		if (West) result.add(map[h][w-1]);
		
		// element in the north-west
		if (North && West) result.add(map[h-1][w-1]);
		
		return result;
	
	}

	private int getDistance(Node nodeA, Node nodeB) {
		// helper method. Provided for your convenience.
		int dstX = Math.abs(nodeA.gridX - nodeB.gridX);
		int dstY = Math.abs(nodeA.gridY - nodeB.gridY);
		if (dstX > dstY)
			return 14 * dstY + 10 * (dstX - dstY);
		return 14 * dstX + 10 * (dstY - dstX);
	}

	public void findPath(Node startNode, Node targetNode)
			throws HeapFullException, HeapEmptyException {
		Heap<Node> openSet = new Heap<>(width * height); 
		// this where we make use of our heaps
		// The rest of your implementation goes here.
		// This method implements A-star path search algorithm.
		// The pseudocode is provided in the appropriate web links.
		// Make sure to use the helper method getNeighbours
		
		// add a starting node to the openset
		startNode.hCost = getDistance(startNode,targetNode);
		startNode.gCost = 0;
		openSet.add(startNode);
		Set<Node> close = new HashSet<Node>();
		// repeating procedure 
		
		while (openSet.isEmpty() == false){
			Node now_node = openSet.removeFirst();
			ArrayList<Node> neighbours = getNeighbours(now_node);
			for(Node n: neighbours){
				// if not walkable , do nothing
				if (n.walkable == false) continue ; 
				// stop the function if find the target
				if (n == targetNode){
					n.parent = now_node;
					return;
				}
				int new_g = now_node.gCost + getDistance(n,now_node);
				// if in the close set, check whether it can be updated
				if(close.contains(n) ){
					if ( new_g < n.gCost){
						close.remove(n);
						n.gCost = new_g;
						n.hCost = getDistance(n,targetNode);
						n.parent = now_node;
						openSet.add(n);
						}
					continue;
				}
				
				// if it is in the openset, update it
				if (openSet.contains(n)){
					if (new_g < n.gCost){
						n.gCost = new_g;
						n.parent = now_node;
						openSet.updateItem(n);
						}
					}
				
				// if not in the close set and not in the openset, add it to the open set
				if (openSet.contains(n) == false){
					n.parent = now_node;
					n.gCost = getDistance(now_node,n) + now_node.gCost;
					n.hCost = getDistance(n,targetNode);
					openSet.add(n);
				}
			}
			// finally add the node to the close set
			close.add(now_node);
		}
		
	}

	public ArrayList<Node> retracePath(Node startNode, Node endNode) {
		Node currentNode = endNode;
	    ArrayList<Node> path = new ArrayList<Node>();
		while (currentNode != startNode && currentNode != null) {
			currentNode.inPath = true;
			path.add(currentNode);
			currentNode = currentNode.parent;
		}
		return path;
	}

	public void move(String direction) {
		// Direction may be: N,S,W,E,NE,NW,SE,SW
		// move the boat 1 cell in the required direction
		
		// check the ability to move up, down, left right
		boolean North,South,East,West;
		int h,w;
		h = boat.gridY;
		w = boat.gridX;
		North = (h - 1) >= 0;
		South = (h + 1) < height;
		East = (w + 1) < width;
		West = (w - 1) >= 0;
		
		// move north
		if (direction.equals("N") && North){
			Node change = map[h-1][w];
			if (change.walkable == false) return;
			map[h-1][w] = boat;
			boat.gridY = h - 1;
			map[h][w] = change;
			change.gridY = h;
		}
		
		// move south
		if (direction.equals("S") && South){
			Node change = map[h+1][w];
			if (change.walkable == false) return;
			map[h+1][w] = boat;
			boat.gridY = h + 1;
			map[h][w] = change;
			change.gridY = h;
		}
		
		// move east
		if (direction.equals("E") && East){
			Node change = map[h][w+1];
			if (change.walkable == false) return;
			map[h][w+1] = boat;
			boat.gridX = w + 1;
			map[h][w] = change;
			change.gridX = w;
		}
		
		// move west
		if (direction.equals("W") && West){
			Node change = map[h][w-1];
			if (change.walkable == false) return;
			map[h][w-1] = boat;
			boat.gridX = w - 1;
			map[h][w] = change;
			change.gridX = w;
		}
		
		// move north-west
		if (direction.equals("NW") && North && West){
			Node change = map[h-1][w-1];
			if (change.walkable == false) return;
			map[h-1][w-1] = boat;
			boat.gridX = w - 1;
			boat.gridY = h - 1;
			map[h][w] = change;
			change.gridY = h;
			change.gridX = w;
		}
		
		// move north-east
		if (direction.equals("NE") && North && East){
			Node change = map[h-1][w+1];
			if (change.walkable == false) return;
			map[h-1][w+1] = boat;
			boat.gridX = w + 1;
			boat.gridY = h - 1;
			map[h][w] = change;
			change.gridY = h;
			change.gridX = w;
		}
		
		// move south-east
		if (direction.equals("SE") && South && East){
			Node change = map[h+1][w+1];
			if (change.walkable == false) return;
			map[h+1][w+1] = boat;
			boat.gridX = w + 1;
			boat.gridY = h + 1;
			map[h][w] = change;
			change.gridY = h;
			change.gridX = w;
		}
		
		// move south-west
		if (direction.equals("SW") && South && West){
			Node change = map[h+1][w - 1];
			if (change.walkable == false) return;
			map[h+1][w-1] = boat;
			boat.gridX = w - 1;
			boat.gridY = h + 1;
			map[h][w] = change;
			change.gridY = h;
			change.gridX = w;
		}
		
		
	}
	
	public Node getTreasure(int range) {
		// range is the range of the sonar
		// if the distance of the treasure from the boat is less or equal that the sonar range,
		// return the treasure node. Otherwise return null.
		int distance = getDistance(boat,treasure);
		if (distance <= range) return treasure;
		return null;
	}

}
