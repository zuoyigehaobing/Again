package heap;

public interface HeapItem extends Comparable<HeapItem>{
	
	public void setHeapIndex(int index);
	public int getHeapIndex();

}
