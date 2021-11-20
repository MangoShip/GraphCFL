#include <atomic>

using namespace std;

class barrier_object {
    public:
        barrier_object() {

        }
        
        void init(int num_threads) {
            count = size = num_threads;
            sense = false;
            threadSense = new atomic_bool[num_threads];
            for (int i = 0; i < num_threads; i++) {
                threadSense[i] = !sense;
            }
        }

        void barrier(int threadId) {
            int mySense = threadSense[threadId].load();
            int position = atomic_fetch_sub(&count, 1);
            if (position == 1) {
                count.store(size);
                sense.store(mySense);
            }
            else {
                while (sense.load() != mySense);
            }
            threadSense[threadId].store(!mySense);
        }

    private:
        atomic_int count;
        int size;
        atomic_bool sense;
        atomic_bool* threadSense;
};