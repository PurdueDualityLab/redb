import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {

  tools = [
    {
      name: 'Regex Database',
      description: 'Query a database of regular expressions to find one that fits your usecase',
      routerFragment: '/query'
    },
    {
      name: 'Some other tool',
      description: 'Do something else',
      routerFragment: '/home'
    }
  ];

  constructor() { }

  ngOnInit(): void {
  }

}
