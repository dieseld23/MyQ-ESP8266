
<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<p align="center">
  <h3 align="center">MyQ-ESP8266</h3>
</p>



<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
* [Roadmap](#roadmap)
* [Contributing](#contributing)
* [License](#license)
* [Contact](#contact)
* [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project

This project is (attempting) to make a library for the ESP8266 for MyQ devices (i.e garage door openers). 

Initial code was based on the [MyQ-API](https://github.com/thomasmunduchira/myq-api) written in javascript. Using [json-streaming-parser2](https://github.com/mrfaptastic/json-streaming-parser2) to parse the stream as the return data is too much for the ESP to use ArduinoJSON.

I mainly started this so I could use [the ESP-8266-AC-Control-Ext](https://github.com/dieseld23/ESP8266-AC-Controller-Ext) project and disable heating or cooling when the garage door is open. But I figured I'd try to make a library that can be reused.

<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/dieseld23/MyQ-ESP8266/issues) for a list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

Project Link: [https://github.com/dieseld23/MyQ-ESP8266](https://github.com/dieseld23/MyQ-ESP8266)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [myq-api](https://github.com/thomasmunduchira/myq-api)
* [json-streaming-parser2](https://github.com/mrfaptastic/json-streaming-parser2)



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/dieseld23/MyQ-ESP8266.svg?style=flat-square
[contributors-url]: https://github.com/dieseld23/MyQ-ESP8266/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/dieseld23/MyQ-ESP8266.svg?style=flat-square
[forks-url]: https://github.com/dieseld23/MyQ-ESP8266/network/members
[stars-shield]: https://img.shields.io/github/stars/dieseld23/MyQ-ESP8266.svg?style=flat-square
[stars-url]: https://github.com/dieseld23/MyQ-ESP8266/stargazers
[issues-shield]: https://img.shields.io/github/issues/dieseld23/MyQ-ESP8266.svg?style=flat-square
[issues-url]: https://github.com/dieseld23/MyQ-ESP8266/issues
[license-shield]: https://img.shields.io/github/license/dieseld23/MyQ-ESP8266.svg?style=flat-square
[license-url]: https://github.com/dieseld23/MyQ-ESP8266/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/dan-maslach-b5b1b857/
