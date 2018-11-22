# Feather TCP watch!
<!-- TODO the components reference to the Adafruit site -->

This is probably the smallest of the configurations we talk about around here.
And the Feather can be fed by a LiPo battery, that makes it autonomous.

### Wiring
![Wiring](./TCP.watch.feather.128x32_bb.png)

### Screenshots

![Splash](./splash.jpg)

![Position](./position.jpg)

![SOG and COG](./sog-cog.jpg)

![Time and Network](./time.net.jpg)

### LiPo Battery tests
Done with `feather.lifespan.test.ino` (See the [README.md](../../feather.lifespan.test/README.md) in its folder), the sketck is running on a Feather powered by a LiPo battery, and POSTs data to a REST service every second or so. Data are formatted with `HH:MM:SS`, that represents the time the Feather has been running.

The last posted data can be obtained from another service `GET /feather/lifespan` by any REST client:
```
 $ curl http://localhost:9999/feather/lifespan
  00:11:37
```

#### Some results
| Capacity | Lifespan |
|---------:|---------:|
|  105 mAh |     1:15 |
|  400 mAh |          |
| 1200 mAh |          |
| 2500 mAh |          |

 ---

Screen on : 1:12:00, keep going => 1:15:00...
End of all 1:16:43
