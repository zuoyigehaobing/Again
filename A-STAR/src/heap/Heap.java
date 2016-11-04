package heap;

public class Heap<T extends HeapItem> {
	
	// Note the T is a parameter representing a type that extends the HeapItem interface
	// This a new way to use inheritance!

	protected T[] items; // Array that is used to store heap items. items[0] is the highest priority element.
	protected int maxHeapSize; // The capacity of the heap
	protected int currentItemCount; // How many elements we have currently on the heap
	
	public Heap(int maxHeapSize) {
		this.maxHeapSize = maxHeapSize;
		items = (T[]) new HeapItem[maxHeapSize];
		currentItemCount = 0; // heap is empty!
	}

	public boolean isEmpty() {
		return currentItemCount == 0;
	}

	public boolean isFull() {
		return currentItemCount == maxHeapSize;
	}

	public void add(T item) throws HeapFullException
	// Adds item T to its correct position on the heap
	{
		if (isFull())
			throw new HeapFullException();
		else {
			item.setHeapIndex(currentItemCount);
			items[currentItemCount] = item;  // the element is added to the bottom
			sortUp(item); // Move the element up to its legitimate place. Check the diagram on the handout!
			currentItemCount++;
		}
	}

	public boolean contains(T item)
	// Returns true if item is on the heap
	// Otherwise returns false
	{
		return items[item.getHeapIndex()].equals(item);
	}

	public int count() {
		return currentItemCount;
	}

	public void updateItem(T item) {
		sortUp(item);
	}

	public T removeFirst() throws HeapEmptyException
	// Removes and returns the element sitting on top of the heap
	{
		if (isEmpty())
			throw new HeapEmptyException();
		else {
			T firstItem = items[0]; // element of top of the heap is stored in firstItem variable
			currentItemCount--;
			items[0] = items[currentItemCount]; //last element moves on top
			items[0].setHeapIndex(0);
			sortDown(items[0]); // move the element to its legitimate position. Please check the diagram on the handout.
			return firstItem;
		}
	}
	
	private void sortUp(T item) {
		// Implement this method according to the diagram on the handout.
		// Also: the indices of children and parent elements satisfy some relationships.
		// The formulas are on the handout.
		if (contains(item) == false) return;
		int index = item.getHeapIndex();
		if (index == 0) return;
		int parent_index = (index - 1) / 2;
		
		//if this item is less then its parents, change the position with its parent, recursion follows
		if (items[index].compareTo(items[parent_index]) < 0){
			T change = items[index];
			items[index] = items[parent_index];
			items[index].setHeapIndex(index);
			items[parent_index] = change;
			items[parent_index].setHeapIndex(parent_index);
			sortUp(change);
		}
		
		else
			return;
	}
	
	private void sortDown(T item) {
		// Implement this method according to the diagram on the handout.
				// Also: the indices of children and parent elements satisfy some relationships.
				// The formulas are on the handout.
		if (contains(item) == false) return;
		int index = item.getHeapIndex();
		int right = index * 2 + 2;
		int left = index * 2 + 1;
		boolean have_l,have_r;
		have_l = (left + 1 <= currentItemCount);  // have left or not
		have_r = (right + 1 <= currentItemCount); // have right or not
		

		if (have_l == false) return; // no left child, do nothing
		if (have_l && have_r == false && items[index].compareTo(items[left]) > 0){
			T change = items[index];
			items[index] = items[left];
			items[index].setHeapIndex(index);
			items[left] = change;
			items[left].setHeapIndex(left);
			sortDown(change);
		}
		
		if (have_r && have_l ){
			boolean g_r = item.compareTo(items[right]) <= 0;
			boolean g_l = item.compareTo(items[left]) <= 0;
			boolean left_big = items[left].compareTo(items[right]) <= 0;
			if (g_r && g_l) return;
			else{
				if(left_big){
					T change = items[index];
					items[index] = items[left];
					items[index].setHeapIndex(index);
					items[left] = change;
					items[left].setHeapIndex(left);
					sortDown(change);
				}
				else{
					T change = items[index];
					items[index] = items[right];
					items[index].setHeapIndex(index);
					items[right] = change;
					items[right].setHeapIndex(right);
					sortDown(change);
				}
			}
			
		}
	}
	
	
	
	// You may implement additional helper methods if desired. Make sure to make them private!
	
	public int maxlevel(){
		int i = 0;
		int j = currentItemCount;
		while (j != 0){
			j = j / 2;
			i++;
		}
		return i;
	}
	
	@Override
	public String toString(){
		String result = "";
		if (isEmpty()) return result;
		int i = 0;
		int j = 1;
		int everytime = 1;
		while ( i != currentItemCount){
			if (everytime > j) {
				result += "\n";
				everytime = 1;
				j = j * 2;
			}
			result += items[i];
			if (everytime % 2 == 0) result += " ";
			else result += "-";
			i++;
			everytime++;
		}
		return result;
	}
	
	
	
}
