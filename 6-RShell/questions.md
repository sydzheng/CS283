1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_The remote client determines when a command's output is fully received from the server by using a special character to mark the end of a logical stream. In this assignment specifically, we used the EOF character to indicate the end of the stream, and loop the receives until that character is reached._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_A networked shell protocol can use special character sequences to mark command boundaries. If it is not handled correctly, a single TCP connection can contain only parts of a command and not be able to process it correctly. If a receiver is waiting for specific data that never arrives, the application can also hang._

3. Describe the general differences between stateful and stateless protocols.

_Stateful protocols remember past interactions between client and server and can store session information on the server. On the other hand, stateless protocols don't store session information between requests as each request contains all information needed to complete it._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_There are certain applications that benefit more from UDP's non-guarantee of packet deliveries or order. Although UDP does not provide these guarantees, internet applications such as YouTube benefit from unreliable packet delivery. If one packet gets dropped while using an application, it will just move on instead of trying to repair this one small mistake._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_Operating systems provides Application Programming Interfaces (APIs)._