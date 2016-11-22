package heap;

public class Str implements HeapItem {
	
	public int heapindex;
	public String s;
	
	public Str(String s){
		this.s = s;
	}
	@Override
	public int compareTo(HeapItem o) {
		// TODO Auto-generated method stub
		if (this.s.compareTo(((Str)o).s) == 0) return 0;
		if (this.s.compareTo(((Str)o).s) > 0) return -1;
		else return 1;
	}

	@Override
	public void setHeapIndex(int index) {
		// TODO Auto-generated method stub
		this.heapindex = index;
		
	}

	@Override
	public int getHeapIndex() {
		// TODO Auto-generated method stub
		return this.heapindex;
	}
	
	@Override
	public String toString(){
		return s;
	}

}
