from collections import deque

class Customer:
    def __init__(self, customer_id, service_time):
        self.customer_id = customer_id
        self.service_time = service_time
        self.wait_time = 0  # Initial wait time is 0

def customer_line(customer, customer_queue):
    already_in_queue = any(c.customer_id == customer.customer_id for c in customer_queue)
    if already_in_queue:
        print(f"Customer with ID {customer.customer_id} is already in the queue.")
        return customer_queue

    # Insert the customer in ascending order based on service time
    inserted = False
    for i, existing_customer in enumerate(customer_queue):
        if customer.service_time < existing_customer.service_time:
            customer_queue.insert(i, customer)
            inserted = True
            break

    if not inserted:
        customer_queue.append(customer)

    # Calculate wait time for each customer
    update_wait_times(customer_queue)

    print(f"Customer with ID {customer.customer_id} is in the queue.")
    print("Customers in the queue:")
    for c in customer_queue:
        print(f"ID: {c.customer_id}, Service Time: {c.service_time} minutes, Wait Time: {c.wait_time} minutes")
    total_wait_time = sum_wait_times(customer_queue)
    print(f"Total wait time is: {total_wait_time}")
    return customer_queue

def sum_wait_times(queue):
    total_time = sum(customer.wait_time for customer in queue)
    return total_time

def update_wait_times(queue):
    for i in range(1, len(queue)):
        queue[i].wait_time = queue[i-1].wait_time + queue[i-1].service_time
