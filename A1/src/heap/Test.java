package heap;

public class Test {

	public static void main(String[] args) throws HeapFullException, HeapEmptyException {
		// TODO Auto-generated method stub
		System.out.println(11/2);
		System.out.println(5/2);
		System.out.println(2/2);
		System.out.println(1/2);
		System.out.println("0".compareTo("1"));
		Str a = new Str("A");
		Str b = new Str("B");
		Str c = new Str("C");
		Str d = new Str("D");
		Str e = new Str("E");
		Str f = new Str("F");
		Str g = new Str("G");
		Str h = new Str("H");
		Str i = new Str("I");
		Str j = new Str("J");
		Str k = new Str("K");
		Str l = new Str("L");
		Str m = new Str("M");
		
		Str[][] h2;
		h2 = new Str[2][2];
		
		Heap trial = new Heap(20);
		trial.items[0] = j;
		trial.items[0].setHeapIndex(0);
		
		trial.items[1] = h;
		trial.items[1].setHeapIndex(1);
		
		trial.items[2] = i;
		trial.items[2].setHeapIndex(2);
		
		trial.items[3] = d;
		trial.items[3].setHeapIndex(3);
		
		trial.items[4] = g;
		trial.items[4].setHeapIndex(4);
		
		trial.items[5] = f;
		trial.items[5].setHeapIndex(5);
		
		trial.items[6] = a;
		trial.items[6].setHeapIndex(6);
		
		trial.items[7] = b;
		trial.items[7].setHeapIndex(7);
		
		trial.items[8] = c;
		trial.items[8].setHeapIndex(8);
		
		trial.items[9] = e;
		trial.items[9].setHeapIndex(9);
		
		
		trial.currentItemCount = 10;
		
		System.out.println(trial + "\n");
		
		trial.removeFirst();
		
		System.out.println(trial + "\n");

trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
trial.removeFirst();
		
		System.out.println(trial + "\n");
		
		
		System.out.println(h2[0][0]);
		
		int index = 56;
		
		int parent_index = (index - 1) / 2;
		
		System.out.println(parent_index);
		System.out.println((index - 1) / 2);
		
	}
	

}
