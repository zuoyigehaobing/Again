package world;

import java.util.ArrayList;

import heap.*;


public class Test2 {
	
	public static void main(String[] args) throws HeapFullException, HeapEmptyException {
		// TODO Auto-generated method stub
		Grid g = new Grid();
		System.out.println(g.drawMap());
		g.findPath(g.boat, g.treasure);
		g.boat.gridX = 0;
		g.boat.gridY = 1;
		//ArrayList<Node> p = g.getNeighbours(g.boat);
		System.out.println("("+g.boat.gridY+" "+g.boat.gridX+")");
		int i=1;
		//for (Node neigh:p){
		//	System.out.println("Neighbour #" + i +" : "+ "(" + neigh.gridY + " " + neigh.gridX + ")");
		//}
		Node n = new Node(false,0,0);
		System.out.println(n.heapIndex);
		
		
		
		
		
	}

}
