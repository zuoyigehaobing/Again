package redbeard;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

import heap.HeapEmptyException;
import heap.HeapFullException;
import world.Grid;
import world.Node;

public class TreasureHunt {

	private final int DEFAULT_SONARS = 3; // default number of available sonars
	private final int DEFAULT_RANGE = 200; // default range of a sonar
	protected Grid islands; // the world where the action happens!
	protected int height, width, landPercent;
	protected int sonars, range; // user defined number of sonars and range
	protected String state; // state of the game (STARTED, OVER)
	protected ArrayList<Node> path; // the path to the treasure!

	public TreasureHunt() {
		// The default constructor
		this.sonars = DEFAULT_SONARS;
		this.range = DEFAULT_RANGE;
		this.islands = new Grid();
		this.state = "STARTED";
	}

	public TreasureHunt(int height, int width, int landPercent, int sonars,
			int range) {
		// The constructor thatuses parameters
		this.sonars = sonars;
		this.range = range;
		this.islands = new Grid(width,height,landPercent);
		this.state = "STARTED";
		
	}

	private void processCommand(String command) throws HeapFullException,
	HeapEmptyException {
// The allowed commands are:
// SONAR to drop the sonar in hope to detect treasure
// GO direction to move the boat in some direction
// For example, GO NW means move the boat one cell up left (if the cell is navigable; if not simply ignore the command)

// if over, return null
if (state.equals("OVER")) return;
// else , discuss

// IF command is SONAR
if (command.equals("SONAR")){
	// if not sonar, then game over
	if (sonars == 0) {
		state = "OVER";
		return;
	}

	// use one sonar
	Node check = islands.getTreasure(range);
	sonars--;

	// if find nothing, return nothing
	if (check == null) return;
	// if find the treasure, game is over and run the A-star algorithm
	if (check != null)
	{
		islands.findPath(islands.getBoat(), check);
		path = islands.retracePath(islands.getBoat(), check);
		state = "OVER";
	}
	// necessary to put a return here, stop the function from going on
	return;
}

// if command is GO **
String head = command.split(" ")[0]; // "GO"
String tail = command.split(" ")[1]; // "Direction"
if (head.equals("GO")){
	// I've checked whether the cell is navigable inside the move method
	islands.move(tail);
	return;
}


}
	public int pathLength() {
		if (path == null)
			return 0;
		else return path.size();
	}

	public String getMap() {
		return islands.drawMap();
	}

	public void play(String pathName) throws FileNotFoundException,
			HeapFullException, HeapEmptyException {
		// Read a batch of commands from a text file and process them.
		Scanner getData = new Scanner(new File(pathName));
		while (getData.hasNextLine()){
			String line = getData.nextLine();
			processCommand(line);
		}
		getData.close();
	}
	
}
