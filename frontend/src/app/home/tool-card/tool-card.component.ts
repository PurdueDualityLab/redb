import { Component, OnInit, ChangeDetectionStrategy, Input } from '@angular/core';

@Component({
  selector: 'app-tool-card',
  templateUrl: './tool-card.component.html',
  styleUrls: ['./tool-card.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class ToolCardComponent implements OnInit {

  @Input()
  title: string = '';

  @Input()
  description: string = '';

  @Input()
  routeFragment: string = '/home';

  constructor() { }

  ngOnInit(): void {
  }

}
