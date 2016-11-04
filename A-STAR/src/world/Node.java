package world;

import heap.HeapItem;

public class Node implements HeapItem {

	protected boolean walkable; // true if this node is available for passage, false if it belongs to an obstacle (i.e. an island)
	protected int gridX, gridY; // gridX runs left to right starting from 0, gridY runs top to bottom starting from 0
	protected int gCost, hCost; // gCost & hCost as explained in the A-star algorithm (check the given web links) 
	protected int heapIndex;    // this is used to implement the methods imposed by HeapItem interface
	protected Node parent;      // points to previous node in the calculated path
	protected boolean inPath;   // true if the node belongs to the calculated path, false otherwise

	public Node(boolean walkable, int gridX, int gridY) {
		this.walkable = walkable;
		this.gridX = gridX;
		this.gridY = gridY;
		this.inPath = false;
	}

	public int getFCost() {
		return gCost + hCost;
	}

	public int getHCost() {
		return hCost;
	}

	@Override
	public int compareTo(HeapItem other) {
		// Your implementation goes here. Similar to Lab4.
		return this.getFCost() - ((Node)other).getFCost();
	}

	@Override
	public void setHeapIndex(int index) {
		heapIndex = index;
	}

	@Override
	public int getHeapIndex() {
		return heapIndex;
	}

	@Override
	public boolean equals(Object other) {
		// Your implementation goes here. Two nodes are equal if they occupy same position in the map.
		if (this.getClass() != other.getClass()) return false;
		else{
			return this.walkable == ((Node)other).walkable &&
					this.gridX == ((Node)other).gridX &&
					this.gridY == ((Node)other).gridY;
		}
	}
	

}
