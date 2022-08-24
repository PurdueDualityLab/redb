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
      name: 'Regex Static Analysis',
      description: 'Statically analyze some regular expressions',
      routerFragment: '/analyze'
    },
    {
      name: 'Regex Rewriter',
      description: 'Rewrite regexes into safter forms (?)',
      routerFragment: '/rewrite'
    }
  ];

  constructor() { }

  ngOnInit(): void {
  }

}
